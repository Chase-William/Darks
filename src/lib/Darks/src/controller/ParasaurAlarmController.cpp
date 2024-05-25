#include "ParasaurAlarmController.h"

#include "../ui/Wrapper.h"

namespace Darks::Controller {
	const std::string ParasaurAlarmConfig::URL_SUBDIRECTORY_NAME = "parasaur-alarm";

	ParasaurAlarmController::ParasaurAlarmController(
		ParasaurAlarmConfig conf
	) :
		conf_(conf)
	{ }

	void ParasaurAlarmController::DisplayCtrlPanel() {
		if (ImGui::TreeNode("Parasaur Alarm Configuration")) {
			if (is_editing_) {
				if (ImGui::Button("Save")) {
					// conf_.Save(post_logs_edit_, enabled_edit_);
					is_editing_ = false;
					ImGui::TreePop();
					return;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					alarm_webhook_edit_ = conf_.alarm_webhook_;
					enabled_edit_ = conf_.enabled_;
					is_editing_ = false;
				}
				ImGui::Checkbox("Enabled", &enabled_edit_);
				ImGui::SameLine();
				UI::InputText("Post Alarm Picture Webhook", &alarm_webhook_edit_);
			}
			else {
				if (ImGui::Button("Edit")) {
					is_editing_ = true;
				}
				ImGui::SameLine();
				ImGui::Text(std::format("Enabled: {}", conf_.enabled_ ? "(Yes)" : "(No)").c_str());
				ImGui::SameLine();
				ImGui::Text(std::format("Using Webhook: {}", conf_.alarm_webhook_.empty() ? "(No)" : "(Yes)").c_str());
			}

			ImGui::TreePop();
		}
	}
}
