#include "BerryFarmerController.h"

namespace Darks::Controllers::Garden {
	BerryFarmerController::BerryFarmerController(
		BerryFarmerConfig conf,
		GlobalHotKeyManager& hotkey_manager,
		SuicideController& suicide_controller,
		SpawnController& spawn_controller,
		InventoryController& inventory_controller,
		MovementController& movement_controller,
		CameraController& camera_controller
	) :
		FloraFarmer(spawn_controller, inventory_controller, movement_controller, camera_controller),
		conf_(conf),
		hotkey_manager_(hotkey_manager),
		suicide_controller_(suicide_controller)
	{ }

	bool BerryFarmerController::StartTimer(
		GlobalTimerManager & timer_manager, 
		QueueSyncInfo& queue_info
	) {
		// Prevent resource leak by accidental overwriting of timer_id_ 
		if (timer_id_) {
			DARKS_ERROR(std::format("Attempted to enter autonomous mode when already in autonomous mode. Leave autonomouse mode and try again to update."));
			return false;
		}

		// Register the farm callback on a repeating timer
		auto result = timer_manager.Register([this, &timer_manager, &queue_info]() {
			std::unique_lock<std::mutex> lock(queue_info.work_queue_mutex_);
			queue_info.work_queue_->push(this);
			lock.unlock();
			// Notify that item added to the queue
			queue_info.queue_var_.notify_all();
			StopTimer(timer_manager);
		},
		conf_.autonomous_berry_farmer_interval_);

		if (result.has_value()) {
			timer_id_ = result.value();
			DARKS_INFO(std::format("Succesfully register timer with id {}.", timer_id_));
			return true;
		}
		else {
			DARKS_INFO("Failed to register timer.");
		}

		return false;
	}

	bool BerryFarmerController::StopTimer(GlobalTimerManager& timer_manager) {
		if (!timer_id_) {
			DARKS_INFO("Attempted to unregister a timer that was already unregistered.");
			return true;
		}
		auto status = timer_manager.Unregister(timer_id_);

		if (status) {
			DARKS_INFO(std::format("Unregistered timer with id {}.", timer_id_));
			timer_id_ = 0; // reset to default		
		}
		else {
			DARKS_WARN(std::format("Failed to unregister timer with id {}.", timer_id_));
		}

		return status;
	}

	void BerryFarmerController::Run(
		SyncInfo& info,
		GlobalTimerManager* const timer_manager,
		QueueSyncInfo* const queue_info,
		MainThreadDispatcher* const dispatcher
		) {
		DARKS_INFO("Running berry farmer controller.");
		running_ = true;

		try {
			bool pause_then_resume_timer = timer_manager && queue_info && dispatcher;
			// Unregister the timer while the task executes if desired
			if (pause_then_resume_timer) {
				dispatcher->Dispatch([this, timer_manager]() {
					return StopTimer(*timer_manager);
				});
			}

			// Main loop for spawning at beds to perform harvest
			const int len = conf_.crops_.size();
			// We need to do two passes over the same crops harvesting each column twice
			for (int harvest_iteration = 0; harvest_iteration < 2; harvest_iteration++) {
				// -- for now we test a citrinal harvest on lower crop column
				for (int col = 0; col < len; col++) {
					// Spawn at crop plot farm (1->4)
					if (!SpawnAtCropBed(info, col)) {
						// Go to next iteration if failed to spawn
						continue;
					}

					// Only wait extra long on the inital render
					if (harvest_iteration == 0 && col == 0) {
						// When rendering for the first time, wait extra long
						info.Wait(45000);
					}
					else {
						// Wait for rendering to complete
						info.Wait(15000);
					}

					// Look all the way up to harvest berries above
					camera_controller_.Pivot(info, CameraDirection::Up, 3000);
					// Must crouch to fit
					movement_controller_.Crouch();

					// Move forward differently depending on the pass
					if (harvest_iteration == 0) {
						// Move foward to first crop plot
						movement_controller_.Move(info, Movement::Forward, 1000);
					}
					else {
						// Move forward to about the half way point and harvest from there
						movement_controller_.Move(info, Movement::Forward, 7000); // ----------------- Change this if second pass starts too late or early
					}

					HarvestCrops(
						info,
						// how many times to harvest from crop plot
						conf_.CROP_HARVEST_ITERATION_COUNT,
						conf_.crops_[col] // crop name to harvest
					);

					// Look down from up
					camera_controller_.Pivot(info, CameraDirection::Down, 5000);

					// Travel to the fridges
					TravelToFridges(info);

					// The first two rows are tinto
					if (col == 0 || col == 1) {
						WalkToCropFridge(info, static_cast<int>(BerryType::Tinto));
						DepositCropsInFridges(info, static_cast<int>(BerryType::Tinto), conf_.FRIDGE_DEPOSIT_ITERATION_COUNT);
					}
					// The second two rows are mejos
					else {
						WalkToCropFridge(info, static_cast<int>(BerryType::Mejo));
						DepositCropsInFridges(info, static_cast<int>(BerryType::Mejo), conf_.FRIDGE_DEPOSIT_ITERATION_COUNT);
					}
				}
			}

			if (pause_then_resume_timer) {
				info.Wait(7000);
				keyboard_controller_.Keystroke(Key::Escape);
				info.Wait(5000);
				if (!suicide_controller_.Suicide(info)) {
					auto msg = "Failed to suicide at the end successfully harvesting all berries.";
					DARKS_WARN(msg);
					throw AutonomousResetRequiredException(msg);
				}

				dispatcher->Dispatch([this, timer_manager, queue_info]() {
					return StartTimer(*timer_manager, *queue_info);
				});				
			}
		}
		catch (std::exception ex) {
			DARKS_ERROR(std::format("An error occured when running the vegetable farmer controller with msg: {}", ex.what()));
			running_ = false;
			throw;
		}
		DARKS_INFO("Berry farmer controller finished execution successfully.");
		running_ = false;
	}


	void BerryFarmerController::DisplayCtrlPanel() {
		if (ImGui::TreeNode(running_ ? "Berry Harvester (running)" : "Berry Harvester")) {

			if (ImGui::Button("Harvest Berries")) {
				if (!running_) {
					hotkey_manager_.Register(conf_.cancel_run_once_hotkey_, [this]() {
						// Initiate shutdown of run once's execution
						run_once_shutdown_ = true;
						run_once_cond_var_.notify_all();
						});

					if (Darks::IO::Window::FocusWindow()) {
						try {
							RunOnce();
						}
						catch (std::exception ex) {
							DARKS_ERROR("An error occured when running the BerryFarmController once on user request.");
						}
					}

					hotkey_manager_.Unregister(conf_.cancel_run_once_hotkey_);
				}
			}

			ImGui::SameLine();
			// Allow immediate harvesting upon startup of the autonomous worker
			ImGui::Checkbox("Run On Startup", &conf_.run_on_autonomous_startup_);
			ImGui::SameLine();
			ImGui::Checkbox("Recurring", &conf_.is_recurring_);

			ImGui::TreePop();
		}		
	}
}