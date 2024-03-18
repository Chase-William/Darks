#pragma once

#include <Windows.h>
#include <vector>

#include "imgui/imgui.h"

#include "ui/DarksImguiWindow.h"
#include "controllers/IDisplayCtrlPanel.h"
#include "controllers/IDisplayHUD.h"
#include "controllers/DisplayController.h"
#include "controllers/IDisposable.h"

#include "AutonomousWorker.h"

class DarkArksApp {
public:
	DarkArksApp() { }

	/// <summary>
	/// Called once for setup.
	/// </summary>
	void Init(HWND hwnd, ImFont* hud_font);

	/// <summary>
	/// Called whenever rendering is going to occur.
	/// </summary>
	/// <param name="io"></param>
	void OnUpdate();
	void OnHotKeyPressed(int id);
	void OnShutdown();

	/// <summary>
	/// Called whenever the visiblity of windows should update.
	/// </summary>
	void OnUpdateVisibility() { ctrl_panel_window_->IsVisible = !ctrl_panel_window_->IsVisible; }
private:
	std::unique_ptr<std::vector<IDisplayCtrlPanel*>> controllers_;

	/* All controls are owned by the DarkArksApp instance, and are passed to one another via a reference. */

	// Hotkeys
	GlobalHotKeyManager hotkey_manager_{}; // Use default
	// Key listening
	GlobalKeyListener key_listener_{}; // Use default
		
	// IO utilities
	MouseController mouse_controller_ = MouseController::New(); // Chance of failure
	KeyboardController keyboard_controller_{};
	
	// In-game utilities
	std::unique_ptr<InventoryController> inventory_controller_;
	std::unique_ptr<SuicideController> suicide_controller_;

	// Automatic operator
	std::unique_ptr<AutonomousWorker> autonomous_worker_;

	// User specific controllers
	/*std::unique_ptr<AutoClickerController> auto_clicker_controller_;
	std::unique_ptr<AutoWalkerController> auto_walker_controller_;*/
	
	// Window display controller
	std::unique_ptr<DisplayController> display_controller_;

	// std::vector<IDisplayCtrlPanel*> controllers_;
	// DisplayController display_controller_;
	// AutonomousWorker auto_worker_;

	std::unique_ptr<DarksImguiWindow> ctrl_panel_window_;

	HWND hwnd_ = NULL;
	ImFont* hud_font_;

	// void RenderControlPanel();
	void RenderHUD();
};

