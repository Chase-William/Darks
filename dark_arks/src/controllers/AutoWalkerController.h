#pragma once

#include <optional>
#include <expected>

#include "imgui/imgui.h"

#include "../Log.h"
#include "IDisplayCtrlPanel.h"
#include "IHotKeyable.h"
#include "KeyboardController.h"
#include "../io/GlobalHotKeyManager.h"
#include "../io/GlobalKeyListener.h"
#include "../io/HotKey.h"
#include "IKeyListener.h"
#include "IDisplayHUD.h"

class AutoWalkerConfig {
public:
	std::optional<HotKey> hotkey_ = { { Key::Num5 } };
	Key walk_forward_key_ = Key::W;
	Key walk_backward_key_ = Key::S;
	Modifiers sprint_key_ = Modifiers::Shift;
};

/// <summary>
/// Provides an automatic walking/sprinting tool.
/// </summary>
class AutoWalkerController : public IDisplayCtrlPanel, public IDisplayHUD, public IHotKeyable, public IKeyListener {
public:
	AutoWalkerController(
		AutoWalkerConfig conf,
		KeyboardController& keyboard_controller,
		GlobalHotKeyManager& hotkey_manager,
		GlobalKeyListener& keyboard_listener);
	
	void DisplayCtrlPanel() override;
	void DisplayHUD() override;
	bool Register() override;
	bool Unregister() override;
	void HandleKeystroke(Key key) override;
	void Dispose() override;

private:
	bool running_ = false;

	AutoWalkerConfig conf_;
	KeyboardController& keyboard_controller_;
	GlobalHotKeyManager& hotkey_manager_;	
	GlobalKeyListener& keyboard_listener_;

	void Walk();
};

