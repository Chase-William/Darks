#include "LootCrateFarmerController.h"

namespace Darks::Controllers::Servers::_2134 {
	LootCrateFarmerController::LootCrateFarmerController(
		LootCrateFarmerConfig conf,
		IO::GlobalHotKeyManager& hotkey_manager,
		InventoryController& inventory_controller,
		CameraController& camera_controller,
		GeneralController& general_controller,
		SpawnController& spawn_controller,
		SuicideController& suicide_controller,
		MovementController& movement_controller
	) :
		conf_(conf),
		spawn_controller_(spawn_controller),
		suicide_controller_(suicide_controller),
		hotkey_manager_(hotkey_manager)
	{
		crates_ = std::make_unique<std::vector<LootCrateInfo*>>();
		crates_->push_back(new DefaultCrate( // euw
			conf_.euw_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.euw_enabled_,
			true,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](const std::string& crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));
		crates_->push_back(new DefaultCrate( // massive 01 - yellow crate
			conf_.massive_01_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.massive_01_enabled_,
			true,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](const std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));
		crates_->push_back(new DefaultCrate( // massive 02 - blue crate
			conf_.massive_02_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.massive_02_enabled_,
			false,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](const std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));

		/* SWAMP RED CRATES */
		crates_->push_back(new SwampCaveCrate( // swamp red 01
			conf_.swamp_red_01_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.swamp_red_01_enabled_,
			true,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](const std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));
		crates_->push_back(new SwampCaveCrate( // swamp red 02
			conf_.swamp_red_02_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.swamp_red_02_enabled_,
			true,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](const std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));

		/* SWAMP YELLOW CRATES */
		crates_->push_back(new SwampCaveCrate( // swamp yellow 01
			conf_.swamp_yellow_01_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.swamp_yellow_01_enabled_,
			false,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));
		crates_->push_back(new SwampCaveCrate( // swamp yellow 01
			conf_.swamp_yellow_02_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.swamp_yellow_02_enabled_,
			false,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));

		crates_->push_back(new SwampCaveCrate( // swamp blue
			conf_.swamp_blue_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.swamp_blue_enabled_,
			true,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));

		crates_->push_back(new SkylordCaveCrate( // skylord
			conf_.skylord_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.skylord_enabled_,
			false,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));
		crates_->push_back(new DefaultCrate( // clever
			conf_.clever_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.clever_crate_enabled_,
			true,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));
		crates_->push_back(new DefaultCrate( // hunter
			conf_.hunter_bed_,
			conf_.screen_shot_loot_rect_,
			&conf_.hunter_crate_enabled_,
			true,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			[this](std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));
		crates_->push_back(new PackCaveCrate( // pack 01
			conf_.pack_bed_01_,
			conf_.screen_shot_loot_rect_,
			&conf_.pack_01_enabled_,
			true,
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			movement_controller,
			[this](std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));
		crates_->push_back(new PackCaveCrate( // pack 02 -- the shallower one
			conf_.pack_bed_02_,
			conf_.screen_shot_loot_rect_,
			&conf_.pack_02_enabled_,
			false, // first one looted gets the double respawn
			inventory_controller,
			camera_controller,
			general_controller,
			spawn_controller,
			suicide_controller,
			movement_controller,
			[this](std::string crate_name, std::vector<char>& jpg_buf) {
				OnCrateOpened(crate_name, jpg_buf);
			}
		));
	}

	void LootCrateFarmerController::DisplayCtrlPanel() {
		if (ImGui::TreeNode(running_ ? "Loot Crate Harvester (running)" : "Loot Crate Harvester")) {

			if (ImGui::Button("Harvest Drops")) {
				if (!running_) {
					hotkey_manager_.Register(conf_.cancel_run_once_hotkey_, [this]() {
						// Initiate shutdown of run once's execution
						run_once_shutdown_ = true;
						run_once_cond_var_.notify_all();
						});

					if (Darks::IO::Window::FocusWindow()) {
						try {
							// Only run if the window was successfully focused	
							RunOnce();
						}
						catch (std::exception ex) {
							DARKS_ERROR("An error occured when running the LootCrateFarmerController once on user request.");
						}
					}

					hotkey_manager_.Unregister(conf_.cancel_run_once_hotkey_);
				}
			}

			ImGui::SameLine();
			// Allow immediate harvesting upon startup of the autonomous worker
			ImGui::Checkbox("Run On Startup", &conf_.run_on_autonomous_startup_);
			ImGui::SameLine();
			// Enable/Disable entire island loot crate farm
			ImGui::Checkbox("Recurring", &conf_.is_recurring_);

			// Display loot crate options
			for (LootCrateInfo* crate : *crates_) {
				ImVec2 cursor_pos = ImGui::GetCursorPos();
				cursor_pos.x += 10;
				ImGui::SetCursorPos(cursor_pos);
				ImGui::Checkbox(crate->GetBedName().c_str(), crate->enabled_);
			}

			ImGui::TreePop();
		}
	}

	bool LootCrateFarmerController::StartTimer(
		GlobalTimerManager& timer_manager,
		QueueSyncInfo& queue_info
	) {
		// Prevent resource leak by accidental overwriting of timer_id_ 
		if (timer_id_) {
			auto msg = std::format("Attempted to enter autonomous mode when already in autonomous mode. Leave autonomouse mode and try again to update.");
			DARKS_ERROR(msg);
			throw std::runtime_error(msg);
		}

		// Register the farm callback on a repeating timer
		auto result = timer_manager.Register([this, &queue_info, &timer_manager]() {
			std::unique_lock<std::mutex> lock(queue_info.work_queue_mutex_);
			queue_info.work_queue_->push(this);
			lock.unlock();
			// Notify that item added to the queue
			queue_info.queue_var_.notify_all();
			// Stop the timer once queue until the job completes, the timer wont start again
			StopTimer(timer_manager);
			},
			conf_.autonomous_farm_interval_);

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

	bool LootCrateFarmerController::StopTimer(GlobalTimerManager& timer_manager) {
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

	void LootCrateFarmerController::Run(
		SyncInfo& info,
		GlobalTimerManager* const timer_manager,
		QueueSyncInfo* const queue_info,
		MainThreadDispatcher* const dispatcher
	) {
		DARKS_INFO("Starting loot crate farm.");
		running_ = true;

		try {
			bool pause_then_resume_timer = timer_manager && queue_info && dispatcher;
			// Unregister the timer while the task executes if desired
			if (pause_then_resume_timer) {
				dispatcher->Dispatch([this, timer_manager]() {
					return StopTimer(*timer_manager);
				});
			}

			// Loop twice to collect double harvestable crates
			for (int i = 0; i < 2; i++) {
				// Loop through and farm each crate
				for (LootCrateInfo* crate : *crates_) {
					if (!crate->enabled_) {
						// Perform a wait even if we are skipping a crate because beds have a 5 minute cooldown and we cant iterate too fast!
						info.Wait(45000);
						continue;
					}

					// Allow all on first iteration, filter second iteration by only crates that should be farmed twice
					if (i == 0 || crate->IsDoubleHarvestable() && i == 1) {
						DARKS_INFO(std::format("Farming crate: {}.", crate->GetBedName()));

						// First we spawn at the crate		
						if (spawn_controller_.Spawn(info, crate->GetBedName())) {
							// Execute the crate farm if spawning was successful
							crate->Execute(info);
						}
						else {
							DARKS_WARN(std::format("Failed to spawn character at bed: {}.", crate->GetBedName()));
							// Clear the searchbar, as the original bed name will pollute the searchbar
							spawn_controller_.Clear(info);
						}
					}
				}
			}

			if (pause_then_resume_timer) {
				dispatcher->Dispatch([this, timer_manager, queue_info]() {
					return StartTimer(*timer_manager, *queue_info);
				});
			}
		}
		catch (std::exception ex) {
			running_ = false;
			throw;
		}



		DARKS_INFO("Ending loot crate farm.");
		running_ = false;
	}
}