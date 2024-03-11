#pragma once

#include "thread"

#include "imgui.h"

#include "../IDisplayCtrlPanel.h"
#include "../IDisplayHUD.h"
#include "../IHotKeyable.h"
#include "../backend/MouseController.h"
#include "../../GlobalHotKeyManager.h"

/// <summary>
/// The configuration used by the auto clicker controller.
/// </summary>
class AutoClickerConfig {
public:
	int interval = 50;
	std::string hotkey_ = "8";
};

/// <summary>
/// Provides an automatic clicking tool.
/// </summary>
class AutoClickerController : public IDisplayCtrlPanel, public IHotKeyable, public IDisplayHUD
{
public:
	AutoClickerController(
		AutoClickerConfig& conf,
		MouseController& mouse_controller, 		
		GlobalHotKeyManager& global_hotkey_manager
	);

	void DisplayCtrlPanel() override;
	void DisplayHUD() override;
	bool Register() override;
	bool Unregister() override;

private:
	MouseController& mouse_controller_;
	AutoClickerConfig& conf_;
	GlobalHotKeyManager& global_hotkey_manager_;

	int hotkey_id_ = 0;
	bool running_ = false;
};

