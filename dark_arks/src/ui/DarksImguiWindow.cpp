#include "DarksImguiWindow.h"

static HWND hwnd_;
static bool is_passthrough_;
static const DWORD WINDOW_EX_STYLE = WS_EX_TRANSPARENT | WS_EX_LAYERED;

DarksImguiWindow::DarksImguiWindow(
	std::vector<IDisplayCtrlPanel*>& controllers,
	AutonomousWorker& auto_worker
) :
	controllers_(controllers),
	auto_worker_(auto_worker)
{ }

void DarksImguiWindow::Init(HWND hwnd) {
	hwnd_ = hwnd;
}

void DarksImguiWindow::Update() {
	ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

	POINT mouse_pos{};
	// Determine if the mouse cursor is above the imgui window and if so, allow it to receive user input until the cursor moves away again
	// Relative to native window
	GetCursorPos(&mouse_pos);
	ImVec2 window_origin_pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize();

	if (window_origin_pos.x <= mouse_pos.x &&
		window_size.x + window_origin_pos.x >= mouse_pos.x && // Mouse is within the imgui window on the x axis
		window_origin_pos.y <= mouse_pos.y &&
		window_size.y + window_origin_pos.y >= mouse_pos.y // Mouse is within the imgui window on the y axis
		) {
		if (is_passthrough_) { // If is passthrough, make it not passthrough and therefore interactable
			EnableInteractivity();
		}
	}
	else if (!is_passthrough_) { // If not passthrough, make it passthrough so clicks can reach the windows behind
		DisableInteractivity();
	}

	bool is_running = auto_worker_.IsRunning();
	if (ImGui::Button(is_running ? "Leave Autonomous Mode" : "Enter Autonomous Mode")) {
		if (is_running)
			auto_worker_.Stop();
		else {
			HWND hwnd = FindWindow(nullptr, L"ArkAscended");
			assert(hwnd);
			//if (!hwnd) {
			//	// Ark isnt running
			//	// Should show some kind of error that isnt as jaring as ahk messagebox
			//}			

			assert(SetForegroundWindow(hwnd));

			//
			// Selecting the imgui window and hiding with the hotkey or via code like this does not update passthrough correct
			// Resulting in the window behind being un-interactable
			//

			// display_controller_.Hide();

			// Bring the ASA to the foreground
			//if (!SetForegroundWindow(hwnd)) {
			//	// could click where the overlay isn't to focus the fullscreen ark window as a backup
			//}
			auto_worker_.Start();
		}
	}

	if (ImGui::Button("Hide Window")) {
		IsVisible = false;
	}

	// Iterate over our controllers and call their rendering methods
	for (IDisplayCtrlPanel* controller : controllers_) {
		controller->DisplayCtrlPanel();
	}

	ImGui::End();
}

void DarksImguiWindow::EnableInteractivity() {
	is_passthrough_ = false;
	SetWindowLongPtrW(hwnd_, GWL_EXSTYLE, 0);
}

void DarksImguiWindow::DisableInteractivity() {
	is_passthrough_ = true;
	SetWindowLongPtrW(hwnd_, GWL_EXSTYLE, WINDOW_EX_STYLE);
}
