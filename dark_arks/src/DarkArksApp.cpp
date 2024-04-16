#include "DarkArksApp.h"

void DarkArksApp::Init(HWND hwnd, MainThreadDispatcher& dispatcher, ImFont* hud_font) {
	hwnd_ = hwnd;
	hud_font_ = hud_font;
	
	auto movement_conf = MovementConfig();
	movement_controller_ = std::make_unique<MovementController>(movement_conf);

	auto inventory_conf = InventoryConfig();
	// If failure when loading from remote -> throw error or use default
	inventory_controller_ = std::make_unique<InventoryController>(inventory_conf);	

	auto camera_conf = CameraConfig();
	camera_controller_ = std::make_unique<CameraController>(camera_conf);

	auto general_conf = GeneralConfig();
	general_controller_ = std::make_unique<GeneralController>(general_conf);

	auto spawn_conf = SpawnConfig();
	spawn_controller_ = std::make_unique<SpawnController>(spawn_conf);

	auto suicide_conf = SuicideConfig();
	// If failure when loading from remote -> throw error or use default
	suicide_controller_ = std::make_unique<SuicideController>(suicide_conf, *spawn_controller_, *inventory_controller_);

	auto process_conf = ProcessConfig();
	process_controller_ = std::make_unique<ProcessController>(process_conf);

	// Loot crate farmer controller
	auto loot_crate_farmer_conf = Darks::Controllers::Island::LootCrateFarmerConfig();
	loot_crate_farmer_controller_ = std::make_unique<Darks::Controllers::Island::LootCrateFarmerController>(
		loot_crate_farmer_conf,
		hotkey_manager_,
		*inventory_controller_,
		*camera_controller_,
		*general_controller_,
		*spawn_controller_,
		*suicide_controller_,
		*movement_controller_
	);

	// Vegetable farmer controller
	auto vegetable_farmer_conf = Darks::Controllers::Garden::VegetableFarmerConf();
	vegetable_farmer_controller_ = std::make_unique<Darks::Controllers::Garden::VegetableFarmerController>(
		vegetable_farmer_conf,
		hotkey_manager_,
		*suicide_controller_,
		*spawn_controller_,
		*inventory_controller_,
		*movement_controller_,
		*camera_controller_
	);

	// Berry farmer controller
	auto berry_farmer_conf = Darks::Controllers::Garden::BerryFarmerConfig();
	berry_farmer_controller_ = std::make_unique<Darks::Controllers::Garden::BerryFarmerController>(
		berry_farmer_conf,
		hotkey_manager_,
		*suicide_controller_,
		*spawn_controller_,
		*inventory_controller_,
		*movement_controller_,
		*camera_controller_
	);	

	auto tribe_log_conf = TibeLogConfig();
	tribe_log_controller_ = std::make_unique<TribeLogController>(tribe_log_conf);

	auto idle_conf = IdleConfig();
	idle_controller_ = std::make_unique<IdleController>(
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
		*loot_crate_farmer_controller_,
		*vegetable_farmer_controller_,
		*berry_farmer_controller_,
		*idle_controller_,
		*process_controller_,
		dispatcher
	);	

	AutoClickerConfig clicker_conf = AutoClickerConfig();
	auto_clicker_controller_ = std::make_unique<AutoClickerController>(clicker_conf, hotkey_manager_);
	auto_clicker_controller_->Register();

	AutoWalkerConfig walker_conf = AutoWalkerConfig();	
	auto_walker_controller_ = std::make_unique<AutoWalkerController>(walker_conf, hotkey_manager_, key_listener_);
	auto_walker_controller_->Register();

	controllers_ = std::make_unique<std::vector<IDisplayCtrlPanel*>>(); // Will the vec attempt to free contents?
	controllers_->push_back(&*auto_clicker_controller_);
	controllers_->push_back(&*auto_walker_controller_);
	controllers_->push_back(&*tribe_log_controller_);

	// Create imgui control panel window
	DarksImguiWindow::Init(hwnd);
	manual_window_ = std::make_unique<ManualModeWindow>(*controllers_, *process_controller_); // manual window
	autonomous_window_ = std::make_unique<AutonomousModeWindow>(
		*autonomous_worker_,
		*loot_crate_farmer_controller_,
		*vegetable_farmer_controller_,
		*berry_farmer_controller_
	);

	// Used to toggle the control panel visiblity
	DisplayControllerConfig display_conf = DisplayControllerConfig();
	display_controller_ = std::make_unique<DisplayController>(
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
		*tribe_log_controller_,
		*loot_crate_farmer_controller_);

	discord_->Start();
}

void DarkArksApp::OnUpdate() {
	// Use the base class function for updating
	DarksImguiWindow::Update({ &*autonomous_window_, &*manual_window_ });

	// Always show HUD... for now
	RenderHUD();
}

void DarkArksApp::OnHotKeyPressed(int id) {
	DARKS_TRACE(fmt::format("Hotkey with id: {:d} pressed.", id));	
	hotkey_manager_.Dispatch(id);
}

void DarkArksApp::OnShutdown() {
	// Perform cleanup on controllers that require it
	for (auto controller : *controllers_) {
		if (IDisposable* disposable = dynamic_cast<IDisposable*>(controller)) {
			disposable->Dispose();
		}		
	}

	display_controller_->Dispose();
	// Do we need to manually destroy each item even if the vec is within a wrapping unique_ptr?
}

void DarkArksApp::RenderHUD() {
	ImGui::PushFont(hud_font_); // Push HUD font to increase size of all rendered text from here until pop
	// Iterate over our controllers and call their rendering methods
	for (IDisplayCtrlPanel* controller : *controllers_) {
		if (IDisplayHUD* overlay = dynamic_cast<IDisplayHUD*>(controller)) {
			overlay->DisplayHUD();
		}
	}
	ImGui::PopFont(); // no longer display bug
}