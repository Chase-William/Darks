#include "ManualModeWindow.h"

namespace Darks::UI {
	ManualModeWindow::ManualModeWindow(
		std::vector<Darks::Controllers::IDisplayCtrlPanel*>& controllers
	) :
		DarksImguiWindow("Manual Mode Window"),
		controllers_(controllers)
	{ }

	void ManualModeWindow::Update() {
		for (Darks::Controllers::IDisplayCtrlPanel* controller : controllers_) {
			controller->DisplayCtrlPanel();
		}
	}

}