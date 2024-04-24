#include "TribeLogController.h"

namespace Darks::Controllers {
	bool TribeLogController::OpenTribeLog(SyncInfo& info, int wait_for_tribe_log_open_poll_interval, int wait_for_tribe_log_open_timeout) const {
		DARKS_INFO("Attempting to open tribe log.");
		if (IsTribeLogOpen()) {
			DARKS_INFO("Tribe log was already open.");
			return true;
		}

		keyboard_controller_.Keystroke(conf_.toggle_tribe_log_key_);
		auto status = info.Wait(*this, wait_for_tribe_log_open_poll_interval, wait_for_tribe_log_open_timeout);
		if (status)
			DARKS_INFO("Successfully opened tribe log.");
		else
			DARKS_INFO("Failed to open tribe log.");
		return status;
	}

	bool TribeLogController::CloseTribeLog(SyncInfo& info, int wait_for_tribe_log_close_poll_interval, int wait_for_tribe_log_close_timeout) const {
		if (!IsTribeLogOpen()) {
			DARKS_INFO("Tribe log was already closed.");
			return true;
		}

		keyboard_controller_.Keystroke(IO::Key::Escape);
		auto status = info.Wait(*this, wait_for_tribe_log_close_poll_interval, wait_for_tribe_log_close_timeout, WAIT_UNTIL_TRIBE_LOG_CLOSED_CODE);
		if (status)
			DARKS_INFO("Successfully closed tribe log.");
		else
			DARKS_INFO("Failed to close tribe log.");
		return status;
	}

	void TribeLogController::DisplayCtrlPanel() {
		ImGui::Text("Tribe Log Controller Text");
		/*if (ImGui::Button("Get Tribelog")) {
			GetScreenshot();
		}*/
	}

	bool TribeLogController::StartPollingTribeLogs(GlobalTimerManager& timer_manager) {
		// Prevent resource leak by accidental overwriting of timer_id_ 
		if (timer_id_) {
			DARKS_ERROR(std::format("Attempted to start polling tribe logs when already polling."));
			return false;
		}

		// Register the farm callback on a repeating timer
		auto result = timer_manager.Register([this, &timer_manager]() {
			if (this->on_log_) {
				DARKS_INFO("Tribe Log Controller fired, capturing screenshot.");
				this->on_log_(this->GetScreenshot());
			}
			},
			conf_.tribe_log_poll_interval_);

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

	bool TribeLogController::StopPollingTribeLogs(GlobalTimerManager& timer_manager) {
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
}