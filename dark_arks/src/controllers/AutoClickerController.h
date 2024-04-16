#ifndef DARKS_CONTROLLER_AUTOCLICKERCONTROLLER_H_
#define DARKS_CONTROLLER_AUTOCLICKERCONTROLLER_H_

#include "thread"

#include "imgui/imgui.h"

#include "IDisplayCtrlPanel.h"
#include "IDisplayHUD.h"
#include "IHotKeyable.h"
#include "MouseController.h"
#include "../io/GlobalHotKeyManager.h"
#include "../io/HotKey.h"

/// <summary>
/// The configuration used by the auto clicker controller.
/// </summary>
class AutoClickerConfig {
public:
	int interval_ = 50;
	std::optional<HotKey> hotkey_ = { { Key::Num8 } };

	/// <summary>
	/// Whether to override and perform right clicks.
	/// </summary>
	bool use_right_click_ = false;
};

/// <summary>
/// Provides an automatic clicking tool.
/// </summary>
class AutoClickerController : public IDisplayCtrlPanel, public IHotKeyable, public IDisplayHUD {
public:
	AutoClickerController(
		AutoClickerConfig conf,	
		GlobalHotKeyManager& hotkey_manager
	);

	void DisplayCtrlPanel() override;
	void DisplayHUD() override;
	bool Register() override;
	bool Unregister() override;
	void Dispose() override;

private:
	AutoClickerConfig conf_;
	MouseController mouse_controller_{};
	GlobalHotKeyManager& hotkey_manager_;

	bool running_ = false;
};

#endif