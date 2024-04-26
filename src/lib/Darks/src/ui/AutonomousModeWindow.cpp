#include "AutonomousModeWindow.h"

namespace Darks::UI {
	AutonomousModeWindow::AutonomousModeWindow(
		AutonomousWorker& auto_worker
	) :
		DarksImguiWindow("Autonomous Mode Window"),
		auto_worker_(auto_worker)
	{ }

	void AutonomousModeWindow::Update() {
		/*loot_crate_farm_controller_.DisplayCtrlPanel();
		vegetable_farmer_controller_.DisplayCtrlPanel();
		berry_farmer_controller_.DisplayCtrlPanel();*/

		auto_worker_.DisplayCtrlPanel();

		// Hide the display when requested.
		/*if (auto_worker_.DisplayCtrlPanel()) {
			this->IsVisible = false;
		}*/
	}
}
