#include "IdleController.h"

#include "../Log.h"
#include "../ui/Wrapper.h"

namespace Darks::Controller {
	const std::string IdleConfig::URL_SUBDIRECTORY_NAME = "idle";	

	const int POLL_INTERVAL = 60000;

	void IdleController::EnterIdle(SyncInfo& info, QueueSyncInfo& queue_sync_info) const {
		DARKS_INFO("Entering idle state.");
		// Spawn at idle bed
		if (!spawn_controller_.Spawn(info, conf_.idle_bed_name_)) {
			auto msg = "Failed to spawn player at idle bed!";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}
		// Give plenty of time to render
		info.Wait(60000);
		// Open tribe log
		if (!tribe_log_controller_.OpenTribeLog(info)) {
			auto msg = "Failed to open tribe log to enter idle state!";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}

		// Register timer on main thread
		if (!dispatcher_.Dispatch([this, queue_sync_info]() {
			// Start polling tribe logs
			return tribe_log_controller_.StartPollingTribeLogs(
				timer_manager_, 
				[this, queue_sync_info]() { // Handle tribe log closed unexpectedly
					// First stop polling tribe logs while we handle this
					auto msg = "Tribe log was unexpectedly closed when attempting to post tribe logs in the idle state.";
					DARKS_WARN(msg);
					tribe_log_controller_.StopPollingTribeLogs(timer_manager_);
					// Notify the queue that it should check it's state again as an error has occured
					// IMPORTANT - We cannot throw exceptions from where as this callback operates on the main thread, no the queue's thread
					queue_sync_info.error_code_ = IDLE_ERROR_UNEXPECTED_TRIBELOG_CLOSED;
					queue_sync_info.queue_var_.notify_all();
				}
			);
		})) {
			auto msg = "Dispatch failed to register timer callback.";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}

		DARKS_INFO("Entered idle state.");
	}

	void IdleController::ExitIdle(SyncInfo& info) const {
		DARKS_INFO("Exiting idle state.");

		// Unregister timer on main thread
		if (!dispatcher_.Dispatch([this]() {
			// Stop polling tribe logs
			return tribe_log_controller_.StopPollingTribeLogs(timer_manager_);
		})) {
			auto msg = "Dispatch failed to unregister timer callback.";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}

		// Close tribe log
		if (!tribe_log_controller_.CloseTribeLog(info)) {
			auto msg = "Failed to close tribe log while in idle state!";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}
		// Look down at bed
		camera_controller_.Pivot(info, CameraDirection::Down, 3000);
		general_controller_.Use();
		info.Wait(5000); // Wait for bed fast-travel screen to popup
		// Check if successful
		if (!spawn_controller_.IsSpawnScreenOpen(SpawnScreen::FastTravelScreen)) {
			auto msg = "Failed to open bed fast-travel screen to exit idle mode!";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}
		DARKS_INFO("Exited idle state.");
	}

	void IdleController::DisplayCtrlPanel() {
		if (ImGui::TreeNode("Idle Configuration")) {
			if (is_editing_) {
				if (ImGui::Button("Save")) {
					conf_.Save(idle_bed_name_edit_);
					is_editing_ = false;
					ImGui::TreePop();
					return;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					idle_bed_name_edit_ = conf_.idle_bed_name_;
					is_editing_ = false;
				}
				UI::InputText("Idle Bed Named", &idle_bed_name_edit_);
			}
			else {
				if (ImGui::Button("Edit")) {
					is_editing_ = true;
				}
				ImGui::SameLine();
				ImGui::Text(std::format("Idle Bed Name: {}", conf_.idle_bed_name_).c_str());
			}

			ImGui::TreePop();
		}
	}

	bool IdleController::StartPolling(QueueSyncInfo& queue_sync_info) {
		// Prevent resource leak by accidental overwriting of timer_id_ 
		if (timer_id_) {
			DARKS_ERROR(std::format("Attempted to start polling tribe logs when already polling."));
			return false;
		}

		// Register the farm callback on a repeating timer
		auto result = timer_manager_.Register([this, &tlc = tribe_log_controller_, &pac = parasaur_alarm_controller_, &queue_sync_info]() {
			
			// Check to see if the tribe log controller is enabled and should therefore post a picture of tribe logs
			if (tlc.IsEnabled()) {
				// Ensure tribe log is actually open
				if (!tlc.IsTribeLogOpen()) {
					// First stop polling tribe logs while we handle this
					auto msg = "Tribe log was unexpectedly closed when attempting to post tribe logs in the idle state.";
					DARKS_WARN(msg);
					// Stop all polling
					this->StopPolling();
					// Notify the queue that it should check it's state again as an error has occured
					// IMPORTANT - We cannot throw exceptions from where as this callback operates on the main thread, no the queue's thread
					queue_sync_info.error_code_ = IDLE_ERROR_UNEXPECTED_TRIBELOG_CLOSED;
					queue_sync_info.queue_var_.notify_all();
					return;
				}
				DARKS_TRACE("Posting tribe logs in idle state.");
				// Post a picture of tribe log
				tlc.PostTribeLogs();
			}

			// Check to see if the parasaur alarm controller is enabled and should check and post picture of alarm if parasaur is raising an alarm
			if (pac.IsEnabled() && pac.IsAlarming()) {
				DARKS_INFO("Parasaur is alarming, posting a screenshot in idle state.");
				pac.PostAlarmScreenshot();				
			}			
		},
		POLL_INTERVAL);

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

	bool IdleController::StopPolling() {
		if (!timer_id_) {
			DARKS_INFO("Attempted to unregister a timer that was already unregistered.");
			return true;
		}
		auto status = timer_manager_.Unregister(timer_id_);

		if (status) {
			DARKS_INFO(std::format("Unregistered timer with id {}.", timer_id_));
			timer_id_ = 0; // reset to default		
		}
		else {
			DARKS_WARN(std::format("Failed to unregister timer with id {}.", timer_id_));
		}

		return status;
	}
}