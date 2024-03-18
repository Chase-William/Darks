#include "DarkArksApp.h"

void DarkArksApp::Init(HWND hwnd, ImFont* hud_font) {
	hwnd_ = hwnd;
	hud_font_ = hud_font;

	auto inventory_conf = InventoryConfig();
	// If failure when loading from remote -> throw error or use default
	inventory_controller_ = std::make_unique<InventoryController>(inventory_conf, keyboard_controller_);

	SuicideConfig suicide_conf = SuicideConfig();
	// If failure when loading from remote -> throw error or use default
	suicide_controller_ = std::make_unique<SuicideController>(suicide_conf, *inventory_controller_, keyboard_controller_, mouse_controller_);

	// Create worker
	autonomous_worker_ = std::make_unique<AutonomousWorker>(*inventory_controller_, *suicide_controller_);	

	AutoClickerConfig clicker_conf = AutoClickerConfig();
	auto auto_clicker_controller = new AutoClickerController(clicker_conf, mouse_controller_, hotkey_manager_);
	auto_clicker_controller->Register();

	AutoWalkerConfig walker_conf = AutoWalkerConfig();	
	auto auto_walker_controller = new AutoWalkerController(walker_conf, keyboard_controller_, hotkey_manager_, key_listener_);
	auto_walker_controller->Register();

	// new std::vector<IDisplayCtrlPanel*>{ auto_walker, auto_clicker }

	controllers_ = std::make_unique<std::vector<IDisplayCtrlPanel*>>();
	controllers_->push_back(auto_clicker_controller);
	controllers_->push_back(auto_walker_controller);

	// Create imgui control panel window
	DarksImguiWindow::Init(hwnd);
	ctrl_panel_window_ = std::make_unique<DarksImguiWindow>(*controllers_, *autonomous_worker_);

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
}

void DarkArksApp::OnUpdate() {
	// Show control panel if toggled on
	// if (display_controller_.Shown())
	if (ctrl_panel_window_->IsVisible)
		ctrl_panel_window_->Update();

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

// void DarkArksApp::RenderControlPanel() {
	//static POINT mouse_pos{};
	//static bool passthrough = true;
	//static const DWORD WINDOW_EX_STYLE = WS_EX_TRANSPARENT | WS_EX_LAYERED;

	//GetCursorPos(&mouse_pos);

	//ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	//// Determine if the mouse cursor is above the imgui window and if so, allow it to receive user input until the cursor moves away again
	//// Relative to native window
	//ImVec2 window_origin_pos = ImGui::GetWindowPos();
	//ImVec2 window_size = ImGui::GetWindowSize();

	//if (window_origin_pos.x <= mouse_pos.x &&
	//	window_size.x + window_origin_pos.x >= mouse_pos.x && // Mouse is within the imgui window on the x axis
	//	window_origin_pos.y <= mouse_pos.y &&
	//	window_size.y + window_origin_pos.y >= mouse_pos.y // Mouse is within the imgui window on the y axis
	//	) {
	//	// ImGui::Text("Hovered!");
	//	if (passthrough) { // Change window attribute to not be passthrough as the user's mouse has entered the imgui window
	//		passthrough = false;
	//		SetWindowLongPtrW(hwnd_, GWL_EXSTYLE, 0);
	//	}
	//}
	//else if (!passthrough) { // Change window attribute to be passthrough as user mouse has left the imgui window
	//	passthrough = true;
	//	SetWindowLongPtrW(hwnd_, GWL_EXSTYLE, WINDOW_EX_STYLE);
	//}

	//bool is_running = auto_worker_.IsRunning();
	//if (ImGui::Button(is_running ? "Leave Autonomous Mode" : "Enter Autonomous Mode")) {
	//	if (is_running)
	//		auto_worker_.Stop();
	//	else {
	//		HWND hwnd = FindWindow(nullptr, L"ArkAscended");
	//		assert(hwnd);
	//		//if (!hwnd) {
	//		//	// Ark isnt running
	//		//	// Should show some kind of error that isnt as jaring as ahk messagebox
	//		//}			

	//		assert(SetForegroundWindow(hwnd));
	//		
	//		//
	//		// Selecting the imgui window and hiding with the hotkey or via code like this does not update passthrough correct
	//		// Resulting in the window behind being un-interactable
	//		//


	//		display_controller_.Hide();														

	//		// Bring the ASA to the foreground
	//		//if (!SetForegroundWindow(hwnd)) {
	//		//	// could click where the overlay isn't to focus the fullscreen ark window as a backup
	//		//}
	//		auto_worker_.Start();
	//	}
	//}

	//// Iterate over our controllers and call their rendering methods
	//for (IDisplayCtrlPanel* controller : controllers_) {
	//	controller->DisplayCtrlPanel();		
	//}	

	// ImGui::End();
// }

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