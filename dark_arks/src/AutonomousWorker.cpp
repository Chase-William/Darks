#include "AutonomousWorker.h"

void AutonomousWorker::Start() {
	// Prevent extra calls
	if (!shutdown_)
		return;	

	queue_info_ = std::make_unique<QueueSyncInfo>(
		&work_queue_, 
		work_queue_mutex_, 
		main_queue_loop_var_
	);

	/* --- Add requested controllers to run when starting --- */
	
	/*
		IMPORTANT: All timers must be registered on the main thread because it never blocks,
			therefore allowing updates to be posted to the queue which is being emptied by a separate blocking thread.
	*/

	if (loot_crate_farmer_controller_.RunOnAutonomousStartup()) { // loot crates on startup
		DARKS_INFO("Island loot crate farmer queued to run on startup.");
		queue_info_->work_queue_->push(&loot_crate_farmer_controller_);
	}
	else if (loot_crate_farmer_controller_.IsRecurring()) { // loot crates on timers
		DARKS_INFO("Island loot crate farmer timer started for autonomous mode.");
		loot_crate_farmer_controller_.StartTimer(timer_manager_, *queue_info_);
	}

	if (vegetable_farmer_controller_.RunOnAutonomousStartup()) { // vegetable harvesting on startup
		DARKS_INFO("Vegetable farmer queued to run on startup.");
		queue_info_->work_queue_->push(&vegetable_farmer_controller_);
	}
	else if (vegetable_farmer_controller_.IsRecurring()) { // harvest veggies on timers
		DARKS_INFO("Vegetable farmer timer started for autonomous mode.");
		vegetable_farmer_controller_.StartTimer(timer_manager_, *queue_info_);
	}
	
	if (berry_farmer_controller_.RunOnAutonomousStartup()) { // berry harvesting on startup
		DARKS_INFO("Berry farmer queue to run on startup.");
		queue_info_->work_queue_->push(&berry_farmer_controller_);
	}
	else if (berry_farmer_controller_.IsRecurring()) { // harvest berries on timers
		DARKS_INFO("Berry farmer timer started for autonomous mode.");
		berry_farmer_controller_.StartTimer(timer_manager_, *queue_info_);
	}

	shutdown_ = false;
	thread_ = std::thread([this]() {
		DARKS_INFO("Starting autonomous worker thread.");
		SyncInfo controller_info = SyncInfo(shutdown_, controller_var_, controller_mutex_);
		
		IQueueable* task = nullptr;

		do { // the main loop for handling exceptions and trying again
			try {

				// Ensure the character is dead as our starting point
				if (!spawn_controller_.IsSpawnScreenOpen(SpawnScreen::DeathScreen)) {
					if (!suicide_controller_.Suicide(controller_info)) {
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
						// Its important we don't lock the queue while we execute the controller
						// Invoke the controller to execute
						task->Run(
							controller_info, 
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
					idle_controller_.EnterIdle(controller_info);

					std::unique_lock<std::mutex> lock(main_queue_loop_mutex_);
					main_queue_loop_var_.wait(lock);

					// Leave idle state when job was enqueued
					idle_controller_.ExitIdle(controller_info);
				} while (!shutdown_); // Loop until shutdown
			}
			catch (AutonomousResetRequiredException ex) {
				DARKS_ERROR(ex.what());

				/* --- Exit Ark& Restart& Join Server-- - */
				process_controller_.Terminate();
				controller_info.Wait(5000); // wait for ark to close, usually quick
				process_controller_.Launch();
				controller_info.Wait(120000); // wait two minutes for ark to start
				process_controller_.JoinServer(controller_info);
				controller_info.Wait(120000); // two minutes			
			}
			catch (std::exception ex) {
				DARKS_ERROR(std::format("Caught exception in autonomous worker thread. {}", ex.what()));
				auto msg = ex.what();
				int i = 0;
			}
		} while (!shutdown_);
							

		// Clean up controller timers
		if (loot_crate_farmer_controller_.IsRecurring())
			loot_crate_farmer_controller_.StopTimer(timer_manager_);
		if (vegetable_farmer_controller_.IsRecurring())
			vegetable_farmer_controller_.StopTimer(timer_manager_);
		if (berry_farmer_controller_.IsRecurring())
			berry_farmer_controller_.StopTimer(timer_manager_);

		DARKS_INFO("Ended autonomous worker thread.");
	});
}

void AutonomousWorker::Stop() {
	// Prevent extra calls
	if (shutdown_)
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

bool AutonomousWorker::DisplayCtrlPanel() {	
	bool is_running = IsRunning();
	if (ImGui::Button(is_running ? "Leave Autonomous Mode" : "Enter Autonomous Mode")) {
		if (is_running)
			Stop();
		else {
			if (Darks::IO::Window::FocusWindow()) {
				Start();
				return true; // Hide this imgui window, updates to passthrough	-- out dated
			}					
		}
	}
	return false;
}