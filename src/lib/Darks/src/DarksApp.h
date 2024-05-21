#ifndef DARKS_DARKSAPP_H_
#define DARKS_DARKSAPP_H_

#include <initializer_list>
#include <memory>

#include "GlobalTimerManager.h"
#include "ui/DarksImguiWindow.h"
#include "ui/AutonomousModeWindow.h"
#include "ui/LoginWindow.h"
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
#include "controller/crate/LootCrateFarmController.h"
#include "io/GlobalKeyListener.h"
#include "io/GlobalHotKeyManager.h"
#include "AutonomousWorker.h"
#include "DarksDiscord.h"
#include "MainThreadDispatcher.h"
#include "OverlayWindowBase.h"

namespace Darks {	

	struct Supplies {
	public:
		Controller::GeneralController& general_controller;
		Controller::MovementController& movement_controller;
		Controller::CameraController& camera_controller;
		Controller::InventoryController& inventory_controller;
		Controller::SuicideController& suicide_controller;
		Controller::SpawnController& spawn_controller;		
		Controller::TribeLogController& tribe_log_controller;
		Controller::ProcessController& process_controller;
		Controller::ServerController& server_controller;
		Controller::Crate::LootCrateFarmController& loot_crate_farm_controller;
		IO::GlobalHotKeyManager& hotkey_manager;
		IO::GlobalKeyListener& key_listener;	
		GlobalTimerManager& timer_manager;		
		MainThreadDispatcher& dispatcher;
		DarksDiscord& discord_bot;
		std::unique_ptr<AutonomousWorker>* worker;
	};

	class DarksApp {
	public:		
		DarksApp(
			std::string distro_token,
			std::optional<std::string> discord_bot_token,
			std::function<std::unique_ptr<std::vector<Controller::IQueueable*>>(Supplies& supplies)> make_queueables
		);

		void Run();		
		
		/// <summary>
		/// Called whenever the visiblity of windows should update.
		/// </summary>
		//void OnUpdateVisibility() {
		//	// Updating both like this could be an issue later on...
		//	manual_window_->IsVisible = !manual_window_->IsVisible;
		//	autonomous_window_->IsVisible = !autonomous_window_->IsVisible;
		//}
	private:		
		/// <summary>
		/// The overlay base window component that handles win32 messages.
		/// </summary>
		std::unique_ptr<Darks::OverlayWindowBase> overlay_window_base_;

		/// <summary>
		/// Contains information needed for interacting with the REST-API.
		/// </summary>
		std::unique_ptr<Darks::ServiceContext> service_state_;

		/* All controls are owned by the DarkArksApp instance, and are passed to one another via a reference. */

		// Hotkeys
		Darks::IO::GlobalHotKeyManager hotkey_manager_{}; // Use default
		// Key listening
		Darks::IO::GlobalKeyListener key_listener_{}; // Use default	
		// Timers
		GlobalTimerManager timer_manager{}; // Use default
		// Main thread dispatcher
		MainThreadDispatcher* dispatcher_ = nullptr;		

		// Discord bot integration
		std::unique_ptr<DarksDiscord> discord_ = nullptr;

		/*
			Required Controllers
		*/

		// Process management
		std::unique_ptr<Darks::Controller::ProcessController> process_controller_;
		// Server management
		std::unique_ptr<Darks::Controller::ServerController> server_controller_;
		std::unique_ptr<Darks::Controller::GeneralController> general_controller_;
		std::unique_ptr<Darks::Controller::MovementController> movement_controller_;
		std::unique_ptr<Darks::Controller::CameraController> camera_controller_;
		std::unique_ptr<Darks::Controller::InventoryController> inventory_controller_;
		std::unique_ptr<Darks::Controller::SuicideController> suicide_controller_;
		std::unique_ptr<Darks::Controller::SpawnController> spawn_controller_;
		// Window display controller
		std::unique_ptr<Darks::Controller::DisplayController> display_controller_;
		std::unique_ptr<Darks::Controller::TribeLogController> tribe_log_controller_;
		std::unique_ptr<Darks::Controller::IdleController> idle_controller_;

		/*
			Optional Controllers
		*/

		std::unique_ptr<Darks::Controller::Crate::LootCrateFarmController> loot_crate_farm_controller_;

		// Automatic operator
		std::unique_ptr<AutonomousWorker> autonomous_worker_;				

		// ImGui windows
		std::unique_ptr<Darks::UI::AutonomousModeWindow> autonomous_window_;
		std::unique_ptr<Darks::UI::LoginWindow> login_window_;

		HWND hwnd_ = NULL;
		// ImFont* hud_font_;

		// void RenderControlPanel();
		void RenderHUD();

		/// <summary>
		/// Called once for setup.
		/// </summary>
		void Init(HWND hwnd, MainThreadDispatcher& dispatcher);

		/// <summary>
		/// Called whenever rendering is going to occur.
		/// </summary>
		/// <param name="io"></param>
		void OnUpdate();
		void OnHotKeyPressed(int id);
		void OnShutdown();
	};
}

#endif