#include "TribeLogController.h"

#include "../ui/Wrapper.h"

namespace Darks::Controller {
	const std::string TribeLogConfig::URL_SUBDIRECTORY_NAME = "tribe-log";

	// const int TRIBE_LOG_POLL_INTERVAL = 60000;

	TribeLogController::TribeLogController(
		TribeLogConfig conf
	) :
		conf_(conf)
	{
		// Copy the remote webhook url into the web hook url modifiable field for the user
		post_logs_edit_ = conf_.post_logs_webhook_;
		enabled_edit_ = conf_.enabled_;
	}

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
		if (ImGui::TreeNode("Tribe Log Logging Configuration")) {
			if (is_editing_) {
				if (ImGui::Button("Save")) {
					conf_.Save(post_logs_edit_, enabled_edit_);
					is_editing_ = false;
					ImGui::TreePop();
					return;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					post_logs_edit_ = conf_.post_logs_webhook_;
					enabled_edit_ = conf_.enabled_;
					is_editing_ = false;
				}
				ImGui::Checkbox("Enabled", &enabled_edit_);
				ImGui::SameLine();
				UI::InputText("Post Tribe-Logs Webhook", &post_logs_edit_);
			}
			else {
				if (ImGui::Button("Edit")) {
					is_editing_ = true;
				}
				ImGui::SameLine();
				ImGui::Text(std::format("Enabled: {}", conf_.enabled_ ? "(Yes)" : "(No)").c_str());
				ImGui::SameLine();
				ImGui::Text(std::format("Using Webhook: {}", conf_.post_logs_webhook_.empty() ? "(No)" : "(Yes)").c_str());
			}

			ImGui::TreePop();
		}		
	}	
}