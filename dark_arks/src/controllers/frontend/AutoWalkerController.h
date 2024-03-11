#pragma once

#include "imgui.h"

#include "../IDisplayCtrlPanel.h"
#include "../IHotKeyable.h"
#include "../backend/KeyboardController.h"
#include "../../GlobalHotKeyManager.h"
#include "../../GlobalKeyboardListener.h"
#include "../IKeyboardListenable.h"
#include "../IDisplayHUD.h"

class AutoWalkerConfig {
public:
	std::string hotkey_ = "5";
	std::string walk_forward_key_ = "W";
	std::string walk_backward_key_ = "S";
	std::string sprint_key_ = "shift";
};

/// <summary>
/// Provides an automatic walking/sprinting tool.
/// </summary>
class AutoWalkerController : public IDisplayCtrlPanel, public IDisplayHUD, public IHotKeyable, public IKeyboardListenable
{
public:
	AutoWalkerController(
		AutoWalkerConfig& conf,
		KeyboardController& keyboard_controller,
		GlobalHotKeyManager& global_hotkey_manager,
		GlobalKeyboardListener& global_keyboard_listener);
	
	void DisplayCtrlPanel() override;
	void DisplayHUD() override;
	bool Register() override;
	bool Unregister() override;
	void HandleKeystroke(Key key) override;

private:
	int hotkey_id_ = 0;
	bool running_ = false;

	Key hotkey_;
	Key walk_foward_key_;
	Key walk_backward_key_;
	Key sprint_key_;

	AutoWalkerConfig& conf_;
	KeyboardController& keyboard_controller_;
	GlobalHotKeyManager& global_hotkey_manager_;	
	GlobalKeyboardListener& global_keyboard_listener_;

	bool Walk();
	void Update(Key key);
};

