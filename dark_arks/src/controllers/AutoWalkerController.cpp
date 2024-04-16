#include "AutoWalkerController.h"

AutoWalkerController::AutoWalkerController(
	AutoWalkerConfig conf,
	GlobalHotKeyManager& global_hotkey_manager,
	GlobalKeyListener& global_keyboard_listener
) :
	conf_(conf),
	hotkey_manager_(global_hotkey_manager),
	keyboard_listener_(global_keyboard_listener)
{ }

void AutoWalkerController::DisplayCtrlPanel() {
	ImGui::Text("Hi, from auto walker controller!");

	// Allow toggling of whether walking or sprinting is enacted when toggled
	// Allow changing of activatio hotkey
	// Allow changing of walk forward key
	// Allow changing of walk backward key
	// Allow changing of sprint mod?
}

void AutoWalkerController::DisplayHUD() {
	if (running_) {
		ImGui::GetBackgroundDrawList()->AddText({ 20, 800 }, ImColor(255, 0, 0), "Auto Walk\0");
	}
}

bool AutoWalkerController::Register() {
	DARKS_TRACE("Registering auto walker hotkey.");

	if (!conf_.hotkey_.has_value()) {
		DARKS_WARN("Attempt to register hotkey that does not have value was averted.");
		return false; // A hotkey to use has not been specified by the user
	}	

	return hotkey_manager_.Register(
		conf_.hotkey_.value(),
		[this]() { Walk(); });
}

bool AutoWalkerController::Unregister() {
	DARKS_TRACE("Unregitering auto walker hotkey.");

	if (!conf_.hotkey_.has_value()) {
		DARKS_WARN("Attempt to unregister a hotkey that does not have a value was averted.");
		return true; // A hotkey for this controller doesn't even exist, nothing to unregister
	}

	if (running_) {
		// Auto walk is running when unreg request was made, therefore toggle it off
		Walk();
	}	

	// Unregister hotkey
	return hotkey_manager_.Unregister(conf_.hotkey_.value());
}

void AutoWalkerController::HandleKeystroke(Key key) {
	// Handle non hotkey keystrokes that should stop the auto walker
	// Use conf
	if (key == conf_.walk_forward_key_) {
		// If the user manually presses the walk key, update state to "toggle off" the auto walk		
		running_ = false;
		// Unregister from listening to keystrokes
		keyboard_listener_ -= this;
	}
	else if (key == conf_.walk_backward_key_) {
		running_ = false;
		keyboard_listener_ -= this;
		// In this case, we need to manually release the walk forward key
		keyboard_controller_.Keystroke(conf_.walk_forward_key_, KeyState::Released);
	}
}

void AutoWalkerController::Dispose()
{
	DARKS_TRACE("Disposing AutoWalkerController.");
	if (conf_.hotkey_.has_value())
		hotkey_manager_.Unregister(conf_.hotkey_.value());	
}

void AutoWalkerController::Walk() {
	DARKS_TRACE("Update to auto walker state requeted.");

	// Key down if now running, otherwise key up
	if (running_ = !running_) {		
		keyboard_controller_.Keystroke(conf_.walk_forward_key_, KeyState::Pressed);

		// Register from lazy keyboard listener
		keyboard_listener_ += this;
	}
	else {
		// Unregister from lazy keyboard listener
		keyboard_listener_ -= this;

		keyboard_controller_.Keystroke(conf_.walk_forward_key_, KeyState::Released);
	}

	DARKS_TRACE(fmt::format("Successfully update walking state to {:b}.", running_));
}
