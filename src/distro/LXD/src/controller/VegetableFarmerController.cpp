#include "VegetableFarmerController.h"

#include <thread>

namespace LXD::Controller {
	VegetableFarmerController::VegetableFarmerController(
		VegetableFarmerConf conf,
		Darks::IO::GlobalHotKeyManager& hotkey_manager,
		Darks::Controller::SuicideController & suicide_controller,
		Darks::Controller::SpawnController& spawn_controller,
		Darks::Controller::InventoryController& inventory_controller,
		Darks::Controller::MovementController& movement_controller,
		Darks::Controller::CameraController& camera_controller
	) :
		FloraFarmer(spawn_controller, inventory_controller, movement_controller, camera_controller),
		conf_(conf),
		suicide_controller_(suicide_controller),
		hotkey_manager_(hotkey_manager)
	{ }

	bool VegetableFarmerController::StartTimer(
		Darks::GlobalTimerManager& timer_manager,
		Darks::Controller::QueueSyncInfo& queue_info
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
		conf_.autonomous_vegetable_farmer_interval_);

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

	bool VegetableFarmerController::StopTimer(Darks::GlobalTimerManager& timer_manager) {
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

	void VegetableFarmerController::Run(
		Darks::SyncInfo& info,
		Darks::GlobalTimerManager* const timer_manager,
		Darks::Controller::QueueSyncInfo* const queue_info,
		Darks::MainThreadDispatcher* const dispatcher
	) {
		DARKS_INFO("Running vegetable farmer controller.");
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
			int len = conf_.crops_.size();
			// -- for now we test a citrinal harvest on lower crop column
			for (int col = 0; col < len; col++) {
				// Spawn at crop plot farm (1->4)
				if (!SpawnAtCropBed(info, col)) {
					// Go to next iteration if fails to spawn
					continue;
				}

				if (col == 0) {
					// When rendering for the first time, wait extra long
					info.Wait(45000);
				}
				else {
					// Wait for rendering to complete
					info.Wait(15000);
				}

				// Look all the way down
				camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Down, 3000);
				// Must crouch to fit
				movement_controller_.Crouch();
				// Move foward an intial amount
				movement_controller_.Move(info, Darks::Controller::Movement::Forward, 1000);

				// Harvest crops
				HarvestCrops(
					info,
					conf_.CROP_HARVEST_ITERATION_COUNT, // how many times to iterate to harvest
					conf_.crops_[col] // crop name to harvest
				);

				// Travel to the fridges
				TravelToFridges(info);

				// Navigate player to the correct fridge line for the given crop
				// Offsets from the citronal base in fridge setup
				WalkToCropFridge(info, col + static_cast<int>(CropType::Citronal));

				// Deposit crops
				DepositCropsInFridges(info, static_cast<int>(CropType::Citronal), conf_.FRIDGE_DEPOSIT_ITERATION_COUNT);
			}

			// Register to resume the timer
			if (pause_then_resume_timer) {
				info.Wait(7000);
				keyboard_controller_.Keystroke(Darks::IO::Key::Escape);
				info.Wait(5000);
				if (!suicide_controller_.Suicide(info)) {
					auto msg = "Failed to suicide at the end successfully harvesting all vegetables.";
					DARKS_WARN(msg);
					throw Darks::AutonomousResetRequiredException(msg);
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
		DARKS_INFO("Vegetable farmer controller finished execution successfully.");
		running_ = false;
	}

	void VegetableFarmerController::DisplayCtrlPanel() {		
		if (ImGui::TreeNode(running_ ? "Vegeteble Harvester (running)" : "Vegeteble Harvester")) {

			if (ImGui::Button("Harvest Vegetables")) {
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
							DARKS_ERROR("An error occured when running the VegetableFarmerController once on user request.");
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