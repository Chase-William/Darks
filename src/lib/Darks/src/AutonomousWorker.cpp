#include "AutonomousWorker.h"

#include <fstream>

namespace Darks {
	void AutonomousWorker::Start() {
		// Prevent redundent attempts to start
		if (IsRunning())
			return;

		SetIsRunning(true);

		queue_info_ = std::make_unique<Controller::QueueSyncInfo>(
			&work_queue_,
			error_code_,
			work_queue_mutex_,
			main_queue_loop_var_
		);

		/*
			IMPORTANT: All timers must be registered on the main thread because it never blocks,
				therefore allowing updates to be posted to the queue which is being emptied by a separate blocking thread.
		*/

		// Iterate over queuebles to ensure run on startups are met
		for (auto& queueable : *queueables_) {
			if (queueable->RunOnStartup()) {
				DARKS_INFO(std::format("Queueable of type: {} was pusing onto work queue on startup.", typeid(queueable).name()));
				queue_info_->work_queue_->push(queueable);
			}
			else {
				DARKS_INFO("Queueable of type: {} had its timer started.", typeid(queueable).name());
				queueable->StartTimer(timer_manager_, *queue_info_);
			}
		}

		shutdown_ = false;
		std::thread([this]() {
			DARKS_INFO("Starting autonomous worker thread.");

			Sleep(500); // Allow the the main thread to successfully hide the overlay before processing begins

			// Attempt to focus the ark window
			if (!Darks::IO::Window::FocusWindow()) {
				DARKS_ERROR("Failed to focus target window for Autonomous Worker. Cleaning up and exiting the autonomous worker.");
				Cleanup();
				SetIsRunning(false);
				return;
			}

			// Click "into" the game with a right click so it will start receive our IO
			Sleep(250);
			mouse_controller_.Click(Controller::ClickType::Right);
			Sleep(250);

			SyncInfo sync_info = SyncInfo(shutdown_, controller_var_, controller_mutex_);

			Controller::IQueueable* task = nullptr;

			do { // the main loop for handling exceptions and attempting to return to a operatable state
				try {

					// Ensure the character is dead as our starting point
					if (!spawn_controller_.IsSpawnScreenOpen(Controller::SpawnScreen::DeathScreen)) {
						if (!suicide_controller_.Suicide(sync_info)) {
							auto msg = "Failed to suicide in order to begin autonomous mode processing.";
							DARKS_ERROR(msg);
							throw AutonomousResetRequiredException(msg);
						}
					}

					do { // the queue loop for processing jobs
						// Check if queue has tasks
						if (!work_queue_.empty())
						{
							{ // Lock the queue when getting the front element and popping it afterwards
								std::scoped_lock<std::mutex> lock(work_queue_mutex_);
								task = work_queue_.front();
							}

							// Before running the job, we need to ensure the character is on the right server and if not, disconnect and connect to the correct server
							//if (auto* specifier = dynamic_cast<Controller::ISpecifyServer*>(task)) {
							//	if (std::string target_serv = specifier->GetServer(); current_server_ != specifier->GetServer()) {
							//		DARKS_INFO(std::format("New job cannot be completed on current server, relog from {} server to target {} server required.", current_server_, target_serv));
							//		// Begin relocation to other server protocol
							//		server_controller_.Disconnect(sync_info);
							//		sync_info.Wait(15000); // Wait 15 seconds to reach main menu screen
							//		server_controller_.Join(sync_info, target_serv);
							//		sync_info.Wait(120000); // Wait 2 minutes for login
							//		if (!spawn_controller_.IsSpawnScreenOpen(Controller::DeathScreen)) {
							//			// If the spawn screen is not open, commit suicide
							//			if (!suicide_controller_.Suicide(sync_info)) {
							//				auto msg = std::format("Failed to suicide in order to begin autonomous mode processing on server {}.", target_serv);
							//				DARKS_ERROR(msg);
							//				throw AutonomousResetRequiredException(msg.c_str());
							//			}										
							//		}
							//	}
							//	else {
							//		DARKS_INFO("New job can be completed on current server.");
							//	}
							//}
							//else {
							//	auto msg = "Task in queue did not meet type requirements. No information can be gathered relating to the server the task should be executed on.";
							//	DARKS_CRITICAL(msg);
							//	throw std::runtime_error(msg);
							//}

							// Its important we don't lock the queue while we execute the controller
							// Invoke the controller to execute
							task->Run(
								sync_info,
								&timer_manager_,
								&this->GetQueueSyncInfo(),
								&this->GetDispatcher());

							{ // After successful completion of the job, pop it from the queue
								std::scoped_lock<std::mutex> lock(work_queue_mutex_);
								work_queue_.pop();
							}
							// Iterate again once finished to check if any jobs had been queued while the current was still running				
							continue;
						}

						// Enter idle state before waiting
						idle_controller_.EnterIdle(sync_info, *queue_info_);

						std::unique_lock<std::mutex> lock(main_queue_loop_mutex_);
						main_queue_loop_var_.wait(lock);

						if (queue_info_->error_code_)
							switch (queue_info_->error_code_) {
							// Handle an issue arrising from the idle controller which occured in a callback on another thread
							case Controller::IDLE_ERROR_UNEXPECTED_TRIBELOG_CLOSED:								
								DARKS_WARN(Controller::IDLE_ERROR_UNEXPECTED_TRIBELOG_CLOSED_MSG);
								throw AutonomousSoftResetRequiredException(Controller::IDLE_ERROR_UNEXPECTED_TRIBELOG_CLOSED_MSG);
							default:
								DARKS_WARN("An error was encountered by a callback running on another thread wait waiting for queue instructions, the code was {}; running **hard** recovery procedure.");
								break;
							}

						// Leave idle state when job was enqueued
						idle_controller_.ExitIdle(sync_info);
					} while (!shutdown_); // Loop until shutdown
				}
				catch (AutonomousSoftResetRequiredException ex) {
					DARKS_WARN(ex.what());
					DARKS_INFO("Attempting soft recovery procedure.");
				}
				catch (AutonomousResetRequiredException ex) {
					DARKS_WARN(ex.what());
					DARKS_INFO("Performing hard recovery procedure.");

					// std::unique_ptr<std::vector<char>> buf = Darks::IO::Screen::GetScreenshot();					

					// Save a screenshot of the screen at the time of error
					/*std::fstream fi;
					fi.open("err.jpg", std::fstream::binary | std::fstream::out);
					fi.write(reinterpret_cast<const char*>(&(*buf)[0]), buf->size() * sizeof(BYTE));
					fi.close();*/

					/* --- Restart Process & Join Server --- */
					process_controller_.Terminate();
					sync_info.Wait(5000); // wait for ark to close, usually quick
					process_controller_.Launch();
					sync_info.Wait(120000); // wait two minutes for ark to start
					// Whenever crashes on server, we want to pickup where we left off, so just rejoin that server
					server_controller_.JoinLast(sync_info);
					sync_info.Wait(120000); // two minutes			
				}
				catch (std::exception ex) {
					DARKS_ERROR(std::format("Caught exception in autonomous worker thread with message: {}.", ex.what()));
				}
			} while (!shutdown_);			

			Cleanup();
			SetIsRunning(false);

			DARKS_INFO("Ended autonomous worker thread.");
		}).detach();
	}

	void AutonomousWorker::Stop() {
		// Prevent redundent attempts to stop
		if (!IsRunning())
			return;

		// Update state
		shutdown_ = true;
		// Notify the controller that may be running to shutdown once it reaches a wait
		std::scoped_lock<std::mutex> controller_lock(controller_mutex_);
		controller_var_.notify_all();
		// Notify the main loop if waiting that shutdown should occur
		std::scoped_lock<std::mutex> main_loop_lock(main_queue_loop_mutex_);
		main_queue_loop_var_.notify_all();
	}

	void AutonomousWorker::DisplayCtrlPanel() {
		bool is_running = IsRunning();
		if (ImGui::Button(is_running ? "Leave Autonomous Mode" : "Enter Autonomous Mode")) {
			if (is_running)
				Stop();
			else
				Start();					
		}

		process_controller_.DisplayCtrlPanel();
		idle_controller_.DisplayCtrlPanel();		
		tribe_log_controller_.DisplayCtrlPanel();
		parasaur_alarm_controller_.DisplayCtrlPanel();

		// Allow contained controllers to display information about themselves
		for (auto& queueable : *queueables_) {
			if (auto iDisplayCtrlPanel = dynamic_cast<Controller::IDisplayCtrlPanel*>(queueable)) {
				iDisplayCtrlPanel->DisplayCtrlPanel();
			}
		}
	}
	
	void AutonomousWorker::Cleanup() {
		// Disable all timers that may be running on exit			
		for (auto& queueable : *queueables_) {
			if (queueable->IsRecurring()) {
				queueable->StopTimer(timer_manager_);
			}
		}
	}
}
