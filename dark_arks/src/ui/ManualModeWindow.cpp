#include "ManualModeWindow.h"

ManualModeWindow::ManualModeWindow(
	std::vector<IDisplayCtrlPanel*>& controllers,
	ProcessController& process_controller
) : 
	DarksImguiWindow("Manual Mode Window"),
	controllers_(controllers),
	process_controller_(process_controller)
{ }

void ManualModeWindow::Update() {
	for (IDisplayCtrlPanel* controller : controllers_) {
		controller->DisplayCtrlPanel();
	}

	// Allow user to manually terminate process
	if (ImGui::Button("Terminate Ark")) {
		process_controller_.Terminate();
	}

	if (ImGui::Button("Launch Ark")) {
		process_controller_.Launch();
	}

	/*if (ImGui::Button("Take Screenshot")) {
		Screen::GetScreenshot(Rect(200, 200, 400, 400));
	}*/	
}
