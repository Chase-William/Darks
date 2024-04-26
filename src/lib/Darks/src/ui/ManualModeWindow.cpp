#include "ManualModeWindow.h"

namespace Darks::UI {
	ManualModeWindow::ManualModeWindow(
		std::vector<Darks::Controller::IDisplayCtrlPanel*>& controller
	) :
		DarksImguiWindow("Manual Mode Window"),
		controllers_(controller)
	{ }

	void ManualModeWindow::Update() {
		for (Darks::Controller::IDisplayCtrlPanel* controller : controllers_) {
			controller->DisplayCtrlPanel();
		}
	}

}