#include "DarkArksApp.h"

namespace Darks {
	void DarkArksApp::Init(HWND hwnd, MainThreadDispatcher& dispatcher, ImFont* hud_font) {
		hwnd_ = hwnd;
		hud_font_ = hud_font;

		auto movement_conf = Darks::Controller::MovementConfig();
		movement_controller_ = std::make_unique<Darks::Controller::MovementController>(movement_conf);

		auto inventory_conf = Darks::Controller::InventoryConfig();
		// If failure when loading from remote -> throw error or use default
		inventory_controller_ = std::make_unique<Darks::Controller::InventoryController>(inventory_conf);

		auto camera_conf = Darks::Controller::CameraConfig();
		camera_controller_ = std::make_unique<Darks::Controller::CameraController>(camera_conf);

		auto general_conf = Darks::Controller::GeneralConfig();
		general_controller_ = std::make_unique<Darks::Controller::GeneralController>(general_conf);

		auto spawn_conf = Darks::Controller::SpawnConfig();
		spawn_controller_ = std::make_unique<Darks::Controller::SpawnController>(spawn_conf);

		auto suicide_conf = Darks::Controller::SuicideConfig();
		// If failure when loading from remote -> throw error or use default
		suicide_controller_ = std::make_unique<Darks::Controller::SuicideController>(suicide_conf, *spawn_controller_, *inventory_controller_);

		auto process_conf = Darks::Controller::ProcessConfig();
		process_controller_ = std::make_unique<Darks::Controller::ProcessController>(process_conf);

		auto server_conf = Darks::Controller::ServerConfig();
		server_controller_ = std::make_unique<Darks::Controller::ServerController>(server_conf);

		// Loot crate farmer controller
		//auto loot_crate_farmer_conf = Darks::Controller::Servers::_2134::LootCrateFarmerConfig();
		//loot_crate_farmer_controller_ = std::make_unique<Darks::Controller::Servers::_2134::LootCrateFarmerController>(
		//	loot_crate_farmer_conf,
		//	hotkey_manager_,
		//	*inventory_controller_,
		//	*camera_controller_,
		//	*general_controller_,
		//	*spawn_controller_,
		//	*suicide_controller_,
		//	*movement_controller_
		//);

		//// Vegetable farmer controller
		//auto vegetable_farmer_conf = Darks::Controller::Servers::_2134::VegetableFarmerConf();
		//vegetable_farmer_controller_ = std::make_unique<Darks::Controller::Servers::_2134::VegetableFarmerController>(
		//	vegetable_farmer_conf,
		//	hotkey_manager_,
		//	*suicide_controller_,
		//	*spawn_controller_,
		//	*inventory_controller_,
		//	*movement_controller_,
		//	*camera_controller_
		//);

		//// Berry farmer controller
		//auto berry_farmer_conf = Darks::Controller::Servers::_2134::BerryFarmerConfig();
		//berry_farmer_controller_ = std::make_unique<Darks::Controller::Servers::_2134::BerryFarmerController>(
		//	berry_farmer_conf,
		//	hotkey_manager_,
		//	*suicide_controller_,
		//	*spawn_controller_,
		//	*inventory_controller_,
		//	*movement_controller_,
		//	*camera_controller_
		//);

		auto tribe_log_conf = Darks::Controller::TibeLogConfig();
		tribe_log_controller_ = std::make_unique<Darks::Controller::TribeLogController>(tribe_log_conf);

		auto idle_conf = Darks::Controller::IdleConfig();
		idle_controller_ = std::make_unique<Darks::Controller::IdleController>(
			idle_conf,
			dispatcher,
			timer_manager,
			*spawn_controller_,
			*tribe_log_controller_,
			*camera_controller_,
			*general_controller_);

		// Create worker
		autonomous_worker_ = std::make_unique<AutonomousWorker>(
			timer_manager,
			*inventory_controller_,
			*suicide_controller_,
			*spawn_controller_,
			*idle_controller_,
			*process_controller_,
			*server_controller_,
			dispatcher
		);

		auto clicker_conf = Darks::Controller::AutoClickerConfig();
		auto_clicker_controller_ = std::make_unique<Darks::Controller::AutoClickerController>(clicker_conf, hotkey_manager_);
		auto_clicker_controller_->Register();

		auto walker_conf = Darks::Controller::AutoWalkerConfig();
		auto_walker_controller_ = std::make_unique<Darks::Controller::AutoWalkerController>(walker_conf, hotkey_manager_, key_listener_);
		auto_walker_controller_->Register();

		controllers_ = std::make_unique<std::vector<Darks::Controller::IDisplayCtrlPanel*>>(); // Will the vec attempt to free contents?
		controllers_->push_back(&*auto_clicker_controller_);
		controllers_->push_back(&*auto_walker_controller_);
		controllers_->push_back(&*tribe_log_controller_);
		controllers_->push_back(&*process_controller_);
		controllers_->push_back(&*server_controller_);

		// Create imgui control panel window
		UI::DarksImguiWindow::Init(hwnd);
		manual_window_ = std::make_unique<Darks::UI::ManualModeWindow>(
			*controllers_
		); // manual window

		autonomous_window_ = std::make_unique<Darks::UI::AutonomousModeWindow>(
			*autonomous_worker_
		);

		// Used to toggle the control panel visiblity
		auto display_conf = Darks::Controller::DisplayControllerConfig();
		display_controller_ = std::make_unique<Darks::Controller::DisplayController>(
			display_conf,
			hotkey_manager_,
			[this]() {
				this->OnUpdateVisibility();
			});
		// Register a callback to toggle the visiblity of imgui windows
		display_controller_->Register();

		auto discord_conf = DarksDiscordConf();
		discord_ = std::make_unique<DarksDiscord>(
			discord_conf,
			*tribe_log_controller_);

		discord_->Start();
	}

	void DarkArksApp::OnUpdate() {
		// Use the base class function for updating
		Darks::UI::DarksImguiWindow::Update({ &*autonomous_window_, &*manual_window_ });

		// Always show HUD... for now
		RenderHUD();
	}

	void DarkArksApp::OnHotKeyPressed(int id) {
		DARKS_TRACE(fmt::format("Hotkey with id: {:d} pressed.", id));
		hotkey_manager_.Dispatch(id);
	}

	void DarkArksApp::OnShutdown() {
		// Perform cleanup on controller that require it
		for (auto controller : *controllers_) {
			if (Darks::Controller::IDisposable* disposable = dynamic_cast<Darks::Controller::IDisposable*>(controller)) {
				disposable->Dispose();
			}
		}

		display_controller_->Dispose();
		// Do we need to manually destroy each item even if the vec is within a wrapping unique_ptr?
	}

	void DarkArksApp::RenderHUD() {
		ImGui::PushFont(hud_font_); // Push HUD font to increase size of all rendered text from here until pop
		// Iterate over our controller and call their rendering methods
		for (Darks::Controller::IDisplayCtrlPanel* controller : *controllers_) {
			if (Darks::Controller::IDisplayHUD* overlay = dynamic_cast<Darks::Controller::IDisplayHUD*>(controller)) {
				overlay->DisplayHUD();
			}
		}
		ImGui::PopFont(); // no longer display bug
	}
}