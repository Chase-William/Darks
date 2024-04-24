#include "DarksImguiWindow.h"

namespace Darks::UI {
	static HWND hwnd_;
	static bool is_passthrough_;
	static const DWORD WINDOW_EX_STYLE = WS_EX_TRANSPARENT | WS_EX_LAYERED;
	static bool hide_all_override = false;

	void DarksImguiWindow::Init(HWND hwnd) {
		hwnd_ = hwnd;		
	}

	void DarksImguiWindow::Update(std::initializer_list<DarksImguiWindow*> windows) {
		// Collect states to see if any of the active imgui windows has a mouse over it and should respond to input
		bool is_imgui_window_hovered = false;
		for (DarksImguiWindow* const window : windows) {
			if (!window->IsVisible || hide_all_override)
				continue; // Skip rendering window if not visible or override to hide all is active
			ImGui::Begin(window->window_name_.c_str());
			// Check if imgui window has the mouse over it
			if (window->IsMouseOver())
				is_imgui_window_hovered = true;

			window->Update(); // Evaluate interactability for all windows

			ImGui::End();
		}

		// Passthrough and is_hovered being equal indicates a window state update is needed
		if (is_passthrough_ == is_imgui_window_hovered) {
			if (is_imgui_window_hovered) {
				// The window should not be passthrough
				EnableInteractivity();
			}
			else {
				// Make the window passthrough
				DisableInteractivity();
			}
		}
	}

	void DarksImguiWindow::Enable() {
		hide_all_override = false;
	}

	void DarksImguiWindow::Disable() {
		hide_all_override = true;
	}

	bool DarksImguiWindow::IsMouseOver() {
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
			return true; // The mouse is within the bounding box of the imgui window
		}
		return false; // The mouse is outside the bounding box of the imgui window
	}

	void DarksImguiWindow::EnableInteractivity() {
		SetWindowLongPtrW(hwnd_, GWL_EXSTYLE, 0);
		is_passthrough_ = false;
	}

	void DarksImguiWindow::DisableInteractivity() {
		SetWindowLongPtrW(hwnd_, GWL_EXSTYLE, WINDOW_EX_STYLE);
		is_passthrough_ = true;
	}
}