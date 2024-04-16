#include "AutoClickerController.h"

AutoClickerController::AutoClickerController(	
	AutoClickerConfig conf,
	GlobalHotKeyManager& global_hotkey_manager
) :	
	conf_(conf),
	hotkey_manager_(global_hotkey_manager)
{ }

void AutoClickerController::DisplayCtrlPanel() {
	ImGui::Text("Auto Clicker");
	ImGui::SameLine();
	ImGui::Checkbox("Use Right Click", &conf_.use_right_click_);
}

void AutoClickerController::DisplayHUD() {
	if (running_) {
		ImGui::GetBackgroundDrawList()->AddText({ 20, 720 }, ImColor(255, 0, 0), "Auto Clicker\0");
	}
}

bool AutoClickerController::Register() {	
	DARKS_TRACE("Registering auto clicker hotkey.");

	if (!conf_.hotkey_.has_value()) {
		DARKS_WARN("Attempt to register hotkey that does not have value was averted.");
		return false; // A Hotkey to use hasnt been specified by the user
	}	

	// A Hotkey to use has been specified by the user
	HotKey hotkey = conf_.hotkey_.value();
	// Register the hotkey_ if a value existed
	return hotkey_manager_.Register(
		hotkey,
		// Create callback function to be executed when the hotkey_ is clicked
		[this]() { // ---------------------------------------------------- Does this lambda need to be freed in unregister?	
			// Toggle the hotkey_
			if (running_ = !running_) {
				// If not running, spool up a thread for auto clicking
				std::thread([this]() {
					// Exit loop and finish when running is false
					while (running_) {
						// Perform mouse click with respect to user preference
						mouse_controller_.Click(conf_.use_right_click_ ? ClickType::Right : ClickType::Left);
						// Sleep
						Sleep(conf_.interval_);
					}						// Perform mouse click
					mouse_controller_.Click();
				}).detach();
			}
		});
}

bool AutoClickerController::Unregister() {
	DARKS_TRACE("Unregistering auto clicker hotkey.");

	if (!conf_.hotkey_.has_value()) {
		DARKS_WARN("Attempt to unregister a hotkey that does not have a value was averted.");
		return true; // A hotkey for this controller doesn't even exist, nothing to unregister
	}
	
	return hotkey_manager_.Unregister(conf_.hotkey_.value());
}

void AutoClickerController::Dispose() {
	DARKS_TRACE("Disposing AutoClickerController.");

	if (conf_.hotkey_.has_value())
		hotkey_manager_.Unregister(conf_.hotkey_.value());
}
