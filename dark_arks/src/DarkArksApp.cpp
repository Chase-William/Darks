#include "DarkArksApp.h"

DarkArksApp::DarkArksApp(std::vector<IDisplayCtrlPanel*> controllers, DisplayController& display_controller) :
	display_controller_(display_controller)
{
	controllers_ = controllers;
}

void DarkArksApp::Init(HWND hwnd, ImFont* hud_font)
{
	hwnd_ = hwnd;
	hud_font_ = hud_font;
}

void DarkArksApp::Update() {
	// Show control panel if toggled on
	if (display_controller_.Show())
		RenderControlPanel();

	// Always show HUD... for now
	RenderHUD();
}

void DarkArksApp::RenderControlPanel() {
	static POINT mouse_pos{};
	static bool passthrough = true;
	static const DWORD WINDOW_EX_STYLE = WS_EX_TRANSPARENT | WS_EX_LAYERED;

	GetCursorPos(&mouse_pos);

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	// Determine if the mouse cursor is above the imgui window and if so, allow it to receive user input until the cursor moves away again
	// Relative to native window
	ImVec2 window_origin_pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize();

	if (window_origin_pos.x <= mouse_pos.x &&
		window_size.x + window_origin_pos.x >= mouse_pos.x && // Mouse is within the imgui window on the x axis
		window_origin_pos.y <= mouse_pos.y &&
		window_size.y + window_origin_pos.y >= mouse_pos.y // Mouse is within the imgui window on the y axis
		) {
		// ImGui::Text("Hovered!");
		if (passthrough) { // Change window attribute to not be passthrough as the user's mouse has entered the imgui window
			passthrough = false;
			SetWindowLongPtrW(hwnd_, GWL_EXSTYLE, 0);
		}
	}
	else if (!passthrough) { // Change window attribute to be passthrough as user mouse has left the imgui window
		passthrough = true;
		SetWindowLongPtrW(hwnd_, GWL_EXSTYLE, WINDOW_EX_STYLE);
	}

	// Iterate over our controllers and call their rendering methods
	for (IDisplayCtrlPanel* controller : controllers_) {
		controller->DisplayCtrlPanel();		
	}	

	ImGui::End();
}

void DarkArksApp::RenderHUD()
{
	ImGui::PushFont(hud_font_); // Push HUD font to increase size of all rendered text from here until pop
	// Iterate over our controllers and call their rendering methods
	for (IDisplayCtrlPanel* controller : controllers_) {
		if (IDisplayHUD* overlay = dynamic_cast<IDisplayHUD*>(controller)) {
			overlay->DisplayHUD();
		}
	}
	ImGui::PopFont(); // no longer display bug
}