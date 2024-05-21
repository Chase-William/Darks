#include "AutonomousModeWindow.h"

namespace Darks::UI {
	AutonomousModeWindow::AutonomousModeWindow(
		AutonomousWorker& auto_worker
	) :
		DarksImguiWindow("Autonomous Mode Window"),
		auto_worker_(auto_worker)
	{
		// Toggle visibility of this window when starting/stopping
		auto_worker.on_started_ = [this]() { IsVisible = false; };
		auto_worker.on_stopped_ = [this]() { IsVisible = true; };
	}

	void AutonomousModeWindow::Update() {
		auto_worker_.DisplayCtrlPanel();
	}
}
