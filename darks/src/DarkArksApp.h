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
#include "controllers/servers/2134/LootCrateFarmerController.h"
#include "controllers/servers/2134/VegetableFarmerController.h"
#include "controllers/servers/2134/BerryFarmerController.h"
#include "controllers/IdleController.h"
#include "controllers/TribeLogController.h"
#include "controllers/ProcessController.h"
#include "controllers/ServerController.h"
#include "io/GlobalKeyListener.h"
#include "io/GlobalHotKeyManager.h"
#include "AutonomousWorker.h"
#include "DarksDiscord.h"
#include "MainThreadDispatcher.h"

namespace Darks {
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
		std::unique_ptr<std::vector<Darks::Controllers::IDisplayCtrlPanel*>> controllers_;

		/* All controls are owned by the DarkArksApp instance, and are passed to one another via a reference. */

		// Hotkeys
		Darks::IO::GlobalHotKeyManager hotkey_manager_{}; // Use default
		// Key listening
		Darks::IO::GlobalKeyListener key_listener_{}; // Use default	
		// Timers
		GlobalTimerManager timer_manager{}; // Use default

		// Process management
		std::unique_ptr<Darks::Controllers::ProcessController> process_controller_;
		// Server management
		std::unique_ptr<Darks::Controllers::ServerController> server_controller_;

		// Discord bot integration
		std::unique_ptr<DarksDiscord> discord_;

		// In-game utilities
		std::unique_ptr<Darks::Controllers::GeneralController> general_controller_;
		std::unique_ptr<Darks::Controllers::MovementController> movement_controller_;
		std::unique_ptr<Darks::Controllers::CameraController> camera_controller_;
		std::unique_ptr<Darks::Controllers::InventoryController> inventory_controller_;
		std::unique_ptr<Darks::Controllers::SuicideController> suicide_controller_;
		std::unique_ptr<Darks::Controllers::SpawnController> spawn_controller_;

		// Manual utilities
		std::unique_ptr<Darks::Controllers::AutoClickerController> auto_clicker_controller_;
		std::unique_ptr<Darks::Controllers::AutoWalkerController> auto_walker_controller_;

		// Automatic utilities
		std::unique_ptr<Darks::Controllers::Servers::_2134::LootCrateFarmerController> loot_crate_farmer_controller_;
		std::unique_ptr<Darks::Controllers::Servers::_2134::VegetableFarmerController> vegetable_farmer_controller_;
		std::unique_ptr<Darks::Controllers::Servers::_2134::BerryFarmerController> berry_farmer_controller_;
		std::unique_ptr<Darks::Controllers::TribeLogController> tribe_log_controller_;

		// Automatic operator
		std::unique_ptr<AutonomousWorker> autonomous_worker_;
		std::unique_ptr<Darks::Controllers::IdleController> idle_controller_;

		// Window display controller
		std::unique_ptr<Darks::Controllers::DisplayController> display_controller_;

		// ImGui windows
		std::unique_ptr<Darks::UI::AutonomousModeWindow> autonomous_window_;
		std::unique_ptr<Darks::UI::ManualModeWindow> manual_window_;

		HWND hwnd_ = NULL;
		ImFont* hud_font_;

		// void RenderControlPanel();
		void RenderHUD();
	};
}

#endif