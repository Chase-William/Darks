#ifndef DARKS_DARKSAPP_H_
#define DARKS_DARKSAPP_H_

#include <initializer_list>

#include "GlobalTimerManager.h"
#include "ui/DarksImguiWindow.h"
#include "ui/AutonomousModeWindow.h"
#include "ui/ManualModeWindow.h"
#include "controller/IDisplayCtrlPanel.h"
#include "controller/IDisplayHUD.h"
#include "controller/DisplayController.h"
#include "controller/IDisposable.h"
#include "controller/GeneralController.h"
#include "controller/MovementController.h"
#include "controller/IdleController.h"
#include "controller/TribeLogController.h"
#include "controller/ProcessController.h"
#include "controller/ServerController.h"
#include "controller/AutoClickerController.h"
#include "controller/AutoWalkerController.h"
#include "io/GlobalKeyListener.h"
#include "io/GlobalHotKeyManager.h"
#include "AutonomousWorker.h"
#include "DarksDiscord.h"
#include "MainThreadDispatcher.h"

namespace Darks {
	
	class DarkArksApp {
	public:
		
		DarkArksApp(
			//std::initializer_list<Darks::Controller::IDisplayCtrlPanel> manuals,
			//std::initializer_list<std::variant<Controller::IDisplayCtrlPanel*, Controller::IQueueable*, Controller::ISpecifyServer*>> autos
		) { }

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
		std::unique_ptr<std::vector<Darks::Controller::IDisplayCtrlPanel*>> controllers_;

		/* All controls are owned by the DarkArksApp instance, and are passed to one another via a reference. */

		// Hotkeys
		Darks::IO::GlobalHotKeyManager hotkey_manager_{}; // Use default
		// Key listening
		Darks::IO::GlobalKeyListener key_listener_{}; // Use default	
		// Timers
		GlobalTimerManager timer_manager{}; // Use default

		// Process management
		std::unique_ptr<Darks::Controller::ProcessController> process_controller_;
		// Server management
		std::unique_ptr<Darks::Controller::ServerController> server_controller_;

		// Discord bot integration
		std::unique_ptr<DarksDiscord> discord_;

		// In-game utilities
		std::unique_ptr<Darks::Controller::GeneralController> general_controller_;
		std::unique_ptr<Darks::Controller::MovementController> movement_controller_;
		std::unique_ptr<Darks::Controller::CameraController> camera_controller_;
		std::unique_ptr<Darks::Controller::InventoryController> inventory_controller_;
		std::unique_ptr<Darks::Controller::SuicideController> suicide_controller_;
		std::unique_ptr<Darks::Controller::SpawnController> spawn_controller_;

		// Manual utilities
		std::unique_ptr<Darks::Controller::AutoClickerController> auto_clicker_controller_;
		std::unique_ptr<Darks::Controller::AutoWalkerController> auto_walker_controller_;

		// Automatic utilities
		/*std::unique_ptr<Darks::Controller::Servers::_2134::LootCrateFarmerController> loot_crate_farmer_controller_;
		std::unique_ptr<Darks::Controller::Servers::_2134::VegetableFarmerController> vegetable_farmer_controller_;
		std::unique_ptr<Darks::Controller::Servers::_2134::BerryFarmerController> berry_farmer_controller_;*/
		std::unique_ptr<Darks::Controller::TribeLogController> tribe_log_controller_;

		// Automatic operator
		std::unique_ptr<AutonomousWorker> autonomous_worker_;
		std::unique_ptr<Darks::Controller::IdleController> idle_controller_;

		// Window display controller
		std::unique_ptr<Darks::Controller::DisplayController> display_controller_;

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