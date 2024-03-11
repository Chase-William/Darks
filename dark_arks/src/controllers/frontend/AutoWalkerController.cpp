#include "AutoWalkerController.h"

AutoWalkerController::AutoWalkerController(
	AutoWalkerConfig& conf,
	KeyboardController& keyboard_controller,
	GlobalHotKeyManager& global_hotkey_manager,
	GlobalKeyboardListener& global_keyboard_listener
) :
	conf_(conf),
	keyboard_controller_(keyboard_controller),
	global_hotkey_manager_(global_hotkey_manager),
	global_keyboard_listener_(global_keyboard_listener)
{
	auto optional_walk_forward_key = ParseKeyStr(conf_.walk_forward_key_);
	auto optional_walk_backward_key = ParseKeyStr(conf_.walk_backward_key_);
	// auto optional_sprint_key = ParseKeyStr(conf_.sprint_key_);
	auto optional_hotkey = ParseKeyStr(conf_.hotkey_);

	// Conf shouldn't contain invalid values	
	assert(optional_walk_forward_key.has_value());
	assert(optional_walk_backward_key.has_value());
	// assert(optional_sprint_key.has_value());
	assert(optional_hotkey.has_value());
		
	walk_foward_key_ = optional_walk_forward_key.value();
	walk_backward_key_ = optional_walk_backward_key.value();
	// sprint_key_ = optional_sprint_key.value();
	hotkey_ = optional_hotkey.value();
}

void AutoWalkerController::DisplayCtrlPanel()
{
	ImGui::Text("Hi, from auto walker controller!");

	// Allow toggling of whether walking or sprinting is enacted when toggled
	// Allow changing of activatio hotkey
	// Allow changing of walk forward key
	// Allow changing of walk backward key
	// Allow changing of sprint mod?
}

void AutoWalkerController::DisplayHUD()
{
	if (running_) {
		ImGui::GetBackgroundDrawList()->AddText({ 20, 800 }, ImColor(255, 0, 0), "Auto Walk\0");
	}
}

bool AutoWalkerController::Register()
{
	auto optional_id = global_hotkey_manager_.Register(
		[this]() { Walk(); },
		hotkey_);
	if (optional_id.has_value()) {
		hotkey_id_ = optional_id.value();
		return true;
	}
	
	return false;
}

bool AutoWalkerController::Unregister()
{
	// Attempted to unreg an already unreg hotkey
	assert(!hotkey_id_);
	// Attempted to unreg a hotkey while a windows hook was already registered
	// assert(low_level_keyboard_hook_);

	// If already unregistered, return true
	if (!hotkey_id_)
		return true;

	if (running_) {
		// Auto walk is running when unreg request was made, therefore toggle it off
		Walk();
	}	

	// Unregister hotkey
	if (global_hotkey_manager_.Unregister(hotkey_id_)) {
		// return success
		return !(hotkey_id_ = 0);
	}
	return false;
}

void AutoWalkerController::HandleKeystroke(Key key)
{
	// Handle non hotkey keystrokes that should stop the auto walker
	// Use conf
	if (key == walk_foward_key_) {
		// If the user manually presses the walk key, update state to "toggle off" the auto walk		
		running_ = false;
		// Unregister from listening to keystrokes
		global_keyboard_listener_ -= this;
	}
	else if (key == walk_backward_key_) {
		running_ = false;
		global_keyboard_listener_ -= this;
		// In this case, we need to manually release the walk forward key
		keyboard_controller_.Keystroke(walk_foward_key_, KeyState::Released);
	}
}

bool AutoWalkerController::Walk()
{
	bool walk_result = false;

	// Key down if now running, otherwise key up
	if (running_ = !running_) {		
		walk_result = keyboard_controller_.Keystroke(walk_foward_key_, KeyState::Pressed);

		// Register from lazy keyboard listener
		global_keyboard_listener_ += this;
	}
	else {
		// Unregister from lazy keyboard listener
		global_keyboard_listener_ -= this;

		walk_result = keyboard_controller_.Keystroke(walk_foward_key_, KeyState::Released);
	}

	return walk_result;
}
