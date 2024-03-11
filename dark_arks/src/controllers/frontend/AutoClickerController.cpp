#include "AutoClickerController.h"

AutoClickerController::AutoClickerController(	
	AutoClickerConfig& conf,
	MouseController& mouse_controller,
	GlobalHotKeyManager& global_hotkey_manager
) :	
	conf_(conf),
	mouse_controller_(mouse_controller),
	global_hotkey_manager_(global_hotkey_manager)
{
	
}

void AutoClickerController::DisplayCtrlPanel()
{
	ImGui::Text("Hi, from auto clicker controller!");
}

void AutoClickerController::DisplayHUD()
{
	if (running_) {
		ImGui::GetBackgroundDrawList()->AddText({ 20, 720 }, ImColor(255, 0, 0), "Auto Clicker\0");
	}
}

bool AutoClickerController::Register()
{	
	// Parse hotkey_ string representation into a Key enum
	auto optional_key = ParseKeyStr(conf_.hotkey_);
	if (optional_key.has_value()) {
		// Register the hotkey_ if a value existed
		auto optional_id = global_hotkey_manager_.Register(
			// Create callback function to be executed when the hotkey_ is clicked
			[this]() { // ---------------------------------------------------- Does this lambda need to be freed in unregister?	
				// Toggle the hotkey_
				if (running_ = !running_) {
					// If not running, spool up a thread for auto clicking
					std::thread([this]() {
						// Exit loop and finish when running is false
						while (running_) {
							// Perform mouse click
							mouse_controller_.Click();
							// Sleep
							Sleep(conf_.interval);
						}						// Perform mouse click
						mouse_controller_.Click();
					}).detach();
				}
			},
			optional_key.value());
		if (optional_id.has_value()) {
			// Store the id of the hotkey_ if a value was returned
			hotkey_id_ = optional_id.value();
			return true;
		}
	}
	// Failure
	return false;
}

bool AutoClickerController::Unregister()
{
	// Assert unreg is not called on an already unregistered hotkey_
	assert(!hotkey_id_);

	if (global_hotkey_manager_.Unregister(hotkey_id_)) {
		// Reset hotkey_ id to 0 if regestration was successful and return true
		return !(hotkey_id_ = 0);
	}
	return false;
}
