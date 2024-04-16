#ifndef DARKS_DARKSAPP_H_
#define DARKS_DARKSAPP_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <vector>

#include "imgui/imgui.h"

#include "GlobalTimerManager.h"
#include "ui/DarksImguiWindow.h"
#include "ui/AutonomousModeWindow.h"
#include "ui/ManualModeWindow.h"
#include "controllers/IDisplayCtrlPanel.h"
#include "controllers/IDisplayHUD.h"
#include "controllers/DisplayController.h"
#include "controllers/IDisposable.h"
#include "controllers/GeneralController.h"
#include "controllers/MovementController.h"
#include "controllers/island/LootCrateFarmerController.h"
#include "controllers/garden/VegetableFarmerController.h"
#include "controllers/garden/BerryFarmerController.h"
#include "controllers/IdleController.h"
#include "controllers/TribeLogController.h"
#include "controllers/ProcessController.h"
#include "io/GlobalKeyListener.h"
#include "io/GlobalHotKeyManager.h"
#include "AutonomousWorker.h"
#include "DarksDiscord.h"
#include "MainThreadDispatcher.h"

class DarkArksApp {
public:
	DarkArksApp() { }

	/// <summary>
	/// Called once for setup.
	/// </summary>
	void Init(HWND hwnd, MainThreadDispatcher& dispatcher, ImFont* hud_font);

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
	void OnUpdateVisibility() { 
		// Updating both like this could be an issue later on...
		manual_window_->IsVisible = !manual_window_->IsVisible;
		autonomous_window_->IsVisible = !autonomous_window_->IsVisible;
	}
private:
	std::unique_ptr<std::vector<IDisplayCtrlPanel*>> controllers_;

	/* All controls are owned by the DarkArksApp instance, and are passed to one another via a reference. */

	// Hotkeys
	GlobalHotKeyManager hotkey_manager_{}; // Use default
	// Key listening
	GlobalKeyListener key_listener_{}; // Use default	
	// Timers
	GlobalTimerManager timer_manager{}; // Use default

	// Process management
	std::unique_ptr<ProcessController> process_controller_;

	// Discord bot integration
	std::unique_ptr<DarksDiscord> discord_;

	// In-game utilities
	std::unique_ptr<GeneralController> general_controller_;
	std::unique_ptr<MovementController> movement_controller_;
	std::unique_ptr<CameraController> camera_controller_;
	std::unique_ptr<InventoryController> inventory_controller_;
	std::unique_ptr<SuicideController> suicide_controller_;
	std::unique_ptr<SpawnController> spawn_controller_;	

	// Manual utilities
	std::unique_ptr<AutoClickerController> auto_clicker_controller_;
	std::unique_ptr<AutoWalkerController> auto_walker_controller_;

	// Automatic utilities
	std::unique_ptr<Darks::Controllers::Island::LootCrateFarmerController> loot_crate_farmer_controller_;
	std::unique_ptr<Darks::Controllers::Garden::VegetableFarmerController> vegetable_farmer_controller_;
	std::unique_ptr<Darks::Controllers::Garden::BerryFarmerController> berry_farmer_controller_;
	std::unique_ptr<TribeLogController> tribe_log_controller_;

	// Automatic operator
	std::unique_ptr<AutonomousWorker> autonomous_worker_;
	std::unique_ptr<IdleController> idle_controller_;

	// Window display controller
	std::unique_ptr<DisplayController> display_controller_;

	// ImGui windows
	std::unique_ptr<AutonomousModeWindow> autonomous_window_;
	std::unique_ptr<ManualModeWindow> manual_window_;

	HWND hwnd_ = NULL;
	ImFont* hud_font_;

	// void RenderControlPanel();
	void RenderHUD();
};

#endif