#include "LootCrateFarmController.h"

#include "../../ui/Wrapper.h"

namespace Darks::Controller::Crate {
	const std::string LootCrateFarmConfig::URL_SUBDIRECTORY_NAME = "loot-crate-farm";
	const int CRATE_FARMING_INTERVAL = 1000 * 60 * 50; // 50 minutes	

	const char* DIRECTION_LBL = "Direction";
	const char* INITIAL_PIVOT_DURATION_LBL = "Initial Pivot Duration";
	const char* PIVOT_STEP_DURATION_LBL = "Pivot Step Duration";
	const char* MAX_PIVOT_STEP_COUNT_LBL = "Max Pivot Step Count";

	LootCrateFarmController::LootCrateFarmController(
		LootCrateFarmConfig conf,
		IO::GlobalHotKeyManager& hotkey_manager,
		InventoryController& inventory_controller,
		CameraController& camera_controller,
		GeneralController& general_controller,
		SpawnController& spawn_controller,
		SuicideController& suicide_controller,
		MovementController& movement_controller
	) :
		conf_(conf),
		hotkey_manager_(hotkey_manager),
		inventory_controller_(inventory_controller),
		camera_controller_(camera_controller),
		general_controller_(general_controller),
		spawn_controller_(spawn_controller),
		suicide_controller_(suicide_controller),
		movement_controller_(movement_controller)
	{
		post_loot_webhook_edit_ = conf.post_loot_webhook_;
		recurring_edit_ = conf.recurring_;
		run_on_startup_edit_ = conf.run_on_startup_;
	}

    void LootCrateFarmController::DisplayCtrlPanel() {
		if (ImGui::TreeNode("Loot Crate Farm Configuration")) {									
			if (is_editing_controls_) {
				if (ImGui::Button("Save")) {
					// Perform save
					conf_.Save(
						run_on_startup_edit_,
						recurring_edit_,
						post_loot_webhook_edit_
					);
					// Leave editing state
					is_editing_controls_ = false;
					ImGui::TreePop();
					return;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					// Reset values
					run_on_startup_edit_ = conf_.run_on_startup_;
					recurring_edit_ = conf_.recurring_;
					post_loot_webhook_edit_ = conf_.post_loot_webhook_;
					// Leave editing state
					is_editing_controls_ = false;
					ImGui::TreePop();
					return;
				}
				// Allow immediate harvesting upon startup of the autonomous worker
				ImGui::Checkbox("Run On Startup", &run_on_startup_edit_);
				ImGui::SameLine();
				// Enable/Disable entire island loot crate farm
				ImGui::Checkbox("Recurring", &recurring_edit_);
				UI::InputText("Post Loot Webhook", &post_loot_webhook_edit_);
			}
			else {
				if (ImGui::Button("Edit")) {
					is_editing_controls_ = true;
					ImGui::TreePop();
					return;
				}
				ImGui::SameLine();
				if (ImGui::Button("Harvest Drops")) {
					if (!IsRunning()) {

						if (Darks::IO::Window::FocusWindow()) {
							try {
								// Only run if the window was successfully focused	
								RunOnce();
							}
							catch (std::exception ex) {
								DARKS_ERROR("An error occured when running the LootCrateFarmerController once on user request.");
							}
						}
					}
				}
				ImGui::SameLine();
				ImGui::Text(std::format("Run On Startup: {}", conf_.run_on_startup_ ? "Yes" : "No").c_str());
				ImGui::SameLine();
				ImGui::Text(std::format("Recurring: {}", conf_.recurring_ ? "Yes" : "No").c_str());
				ImGui::SameLine();
				ImGui::Text(std::format("Using Webhook: {}", post_loot_webhook_edit_.empty() ? "No" : "Yes").c_str());								
			}			

			if (ImGui::TreeNode("Show Crate Stations")) {
				if (is_editing_crates_) {
					// Save the edit and return to non-editing mode
					if (ImGui::Button("Save")) {
						// Save will handle cleanup
						conf_.Save(temp_crates_);
						CleanUpTemporaries();
						is_editing_crates_ = false;
						ImGui::TreePop();
						ImGui::TreePop();
						return;
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel")) {
						// If the user is leaving edit mode without saving, delete coppies and clear collection
						CleanUpTemporaries();
						is_editing_crates_ = false;
						ImGui::TreePop();
						ImGui::TreePop();
						return;
					}
					ImGui::SameLine();
					ImGui::Text("Create crate stations to match your in-game drop setup.");
					ImGui::SameLine();
					// Create a new crate station instance
					if (ImGui::Button("New")) {
						temp_crates_.push_back(new CrateConfig());
					}

					CrateConfig* crate = nullptr;
					// Display editing ui for configuration
					for (auto iter = temp_crates_.begin(); iter != temp_crates_.end(); iter++) {
						ImGui::PushID(iter._Ptr); // Push id onto stack to be used when generating id
						crate = *iter;

						ImGui::SeparatorText(crate->bed_name_.c_str());

						// Delete this item
						if (ImGui::Button("Delete")) {
							temp_crates_.erase(iter);
							delete crate;
							crate = nullptr;
							ImGui::PopID();
							break; // Break as iterator has been invalided at this point
						}

						ImGui::SameLine();
						ImGui::Checkbox("Enabled", &crate->enabled_);
						ImGui::SameLine();
						ImGui::Checkbox("Double Harvestable", &crate->is_double_harvestable_);
						ImGui::SameLine();
						ImGui::Checkbox("Automatically Dies", &crate->wait_until_automatic_death_);
						ImGui::SameLine();
						ImGui::SetNextItemWidth(120);
						ImGui::InputInt("Load Delay", &crate->load_delay_, 1000);
						ImGui::SameLine();
						ImGui::SetNextItemWidth(150);
						UI::InputText("Bed Name [A-Z0-9WhiteSpace]", &crate->bed_name_);
						ImGui::SameLine();

						const int width = 200;

						ImGui::SetNextItemWidth(120);
						ImGui::Combo("Bed To Crate Navigator", reinterpret_cast<int*>(&crate->to_crate_navigator_type_), "None\0SwanDive");

						if (ImGui::BeginTable("Inventory Acquisition Movements", 2, ImGuiTableFlags_Resizable)) {
							ImGui::PushItemWidth(width);

							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::Text("Open crate movement configuration");
							ImGui::TableNextColumn();
							ImGui::Text("Open deposit container movement configuration");

							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::Combo(DIRECTION_LBL, reinterpret_cast<int*>(&crate->acquire_crate_.direction), "Left\0Up\0Right\0Down");
							ImGui::TableNextColumn();
							ImGui::Combo(std::format("{}##1", DIRECTION_LBL).c_str(), reinterpret_cast<int*>(&crate->acquire_deposit_container_.direction), "Left\0Up\0Right\0Down");

							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::InputInt(INITIAL_PIVOT_DURATION_LBL, &crate->acquire_crate_.initial_pivot_duration, 100);
							ImGui::TableNextColumn();
							ImGui::InputInt(std::format("{}##1", INITIAL_PIVOT_DURATION_LBL).c_str(), &crate->acquire_deposit_container_.initial_pivot_duration, 100);

							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::InputInt(PIVOT_STEP_DURATION_LBL, &crate->acquire_crate_.pivot_step_duration, 100);
							ImGui::TableNextColumn();
							ImGui::InputInt(std::format("{}##1", PIVOT_STEP_DURATION_LBL).c_str(), &crate->acquire_deposit_container_.pivot_step_duration, 100);

							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::InputInt(MAX_PIVOT_STEP_COUNT_LBL, &crate->acquire_crate_.max_pivot_step_count, 5);
							ImGui::TableNextColumn();
							ImGui::InputInt(std::format("{}##1", MAX_PIVOT_STEP_COUNT_LBL).c_str(), &crate->acquire_deposit_container_.max_pivot_step_count, 5);

							ImGui::PopItemWidth();
							ImGui::EndTable();
						}

						ImGui::PopID();
					}
				}
				else {
					// Enable editing
					if (ImGui::Button("Edit")) {
						// Only when the user is entering edit mode perform a deep copy of the config data to mutate
						if (!is_editing_crates_) {
							for (auto& crate : conf_.crates_) {
								temp_crates_.push_back(new CrateConfig(crate));
							}
						}
						is_editing_crates_ = true;
						ImGui::TreePop();
						ImGui::TreePop();
						return;
					}

					if (conf_.crates_.empty()) {
						ImGui::SameLine();
						ImGui::Text("Empty, Click *Edit* to create new drop stations.");
					}
					else {
						for (auto& crate : conf_.crates_) {
							ImGui::SeparatorText(crate.bed_name_.c_str());

							ImGui::Text(std::format("Enabled: {}", crate.enabled_ ? "(Yes)" : "(No)").c_str());
							ImGui::SameLine();
							ImGui::Text(std::format("Double Harvesting: {}", crate.is_double_harvestable_ ? "(Yes)" : "(No)").c_str());
							ImGui::SameLine();
							ImGui::Text(std::format("Load Delay: {}", crate.load_delay_, "(milli)").c_str());
							ImGui::SameLine();
							ImGui::Text(std::format("Bed Name: {}", crate.bed_name_).c_str());
							ImGui::SameLine();
							ImGui::Text(std::format("Bed to Crate Navigator: {}", CRATE_STATION_NAVIGATOR_TYPES[static_cast<int>(crate.to_crate_navigator_type_)]).c_str());

							if (ImGui::BeginTable("Inventory Aquisition Movements", 3, ImGuiTableFlags_Resizable)) {

								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::TableNextColumn();
								ImGui::Text("Open crate movement configuration");
								ImGui::TableNextColumn();
								ImGui::Text("Open deposit container movement configuration");

								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Pivot Direction");
								ImGui::TableNextColumn();
								ImGui::Text(CAMERA_DIRECTION[static_cast<int>(crate.acquire_crate_.direction)].c_str());
								ImGui::TableNextColumn();
								ImGui::Text(CAMERA_DIRECTION[static_cast<int>(crate.acquire_deposit_container_.direction)].c_str());

								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Initial Pivot Duration");
								ImGui::TableNextColumn();
								ImGui::Text(std::format("{}", crate.acquire_crate_.initial_pivot_duration).c_str());
								ImGui::TableNextColumn();
								ImGui::Text(std::format("{}", crate.acquire_deposit_container_.initial_pivot_duration).c_str());

								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Pivot Step Duration");
								ImGui::TableNextColumn();
								ImGui::Text(std::format("{}", crate.acquire_crate_.pivot_step_duration).c_str());
								ImGui::TableNextColumn();
								ImGui::Text(std::format("{}", crate.acquire_deposit_container_.pivot_step_duration).c_str());

								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Max Pivot Step Count");
								ImGui::TableNextColumn();
								ImGui::Text(std::format("{}", crate.acquire_crate_.max_pivot_step_count).c_str());
								ImGui::TableNextColumn();
								ImGui::Text(std::format("{}", crate.acquire_deposit_container_.max_pivot_step_count).c_str());

								ImGui::EndTable();
							}
						}
					}
				}

				ImGui::TreePop();
			}										

			ImGui::TreePop();
		}
    }

    bool LootCrateFarmController::StartTimer(GlobalTimerManager& timer_manager, QueueSyncInfo& queue_info) {       
		// Prevent resource leak by accidental overwriting of timer_id_ 
		if (timer_id_) {
			auto msg = std::format("Attempted to enter autonomous mode when already in autonomous mode. Leave autonomouse mode and try again to update.");
			DARKS_INFO(msg);
			return true;
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
		CRATE_FARMING_INTERVAL);

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

	bool LootCrateFarmController::StopTimer(Darks::GlobalTimerManager& timer_manager) {
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
	
	void LootCrateFarmController::Run(
		SyncInfo& info, 
		GlobalTimerManager* const timer_manager, 
		QueueSyncInfo* const queue_info, 
		MainThreadDispatcher* const dispatcher
	) {
		DARKS_INFO("Starting loot crate farm.");
		
		CreateRuntimeObjects();

		SetIsRunning(true);

		try {
			// Only puase and resume if these arguments have been provided actual values
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
				for (auto& crate : crates_) {
					if (!crate->IsEnabled()) {
						// Perform a wait even if we are skipping a crate because beds have a 5 minute cooldown and we cant iterate too fast!
						info.Wait(45000);
						continue;
					}

					// Allow all on first iteration, filter second iteration by only crates that should be farmed twice
					if (i == 0 || crate->IsDoubleHarvestable() && i == 1) {
						auto bed_name = crate->GetBedName();
						// First we spawn at the crate		
						if (spawn_controller_.Spawn(info, bed_name)) {
							// Execute the crate farm if spawning was successful
							crate->Execute(info);
						}
						else {
							DARKS_WARN(std::format("Failed to spawn character at bed: {}.", bed_name));
							// Clear the searchbar, as the original bed name will pollute the searchbar
							spawn_controller_.Clear(info);
						}
					}
				}
			}

			// Only restart the timer if (read above for pause_then_resume_timer) and recurring is still set to true
			if (pause_then_resume_timer && conf_.recurring_) {
				dispatcher->Dispatch([this, timer_manager, queue_info]() {
					return StartTimer(*timer_manager, *queue_info);
				});
			}
		}
		catch (std::exception ex) {
			SetIsRunning(false);
			CleanupRuntimeObjects();
			throw;
		}

		DARKS_INFO("Ending loot crate farm.");
		SetIsRunning(false);
		CleanupRuntimeObjects();
	}
	
	void LootCrateFarmController::CreateRuntimeObjects() {
		DARKS_INFO("Creating crate objects to use in loot crate farm run instance.");
		// Iterate over all crates configs and create the corresponding crate station object
		for (auto& crate : conf_.crates_) {
			switch (crate.station_type_) {
			case StationType::Default: {
				// Crate str to be copied into functional expression
				crates_.push_back(std::make_unique<Station::DefaultCrateStation>(
					crate,
					inventory_controller_,
					camera_controller_,
					general_controller_,
					spawn_controller_,
					suicide_controller_,
					(on_crate_ ? ([this](std::unique_ptr<CrateMessage> msg) { OnCrate(std::move(msg)); }) : std::optional<std::function<void(std::unique_ptr<CrateMessage> msg)>>()),
					GetNavigator(crate.to_crate_navigator_type_) // Get a pointer to base type of navigator
				));
			}
			break;
			default:
				break;
				// Allow user to create custom crate station?
			}
		}
	}

	void LootCrateFarmController::CleanupRuntimeObjects() {
		crates_.clear();
	}
}