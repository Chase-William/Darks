#include "AutonomousModeWindow.h"

namespace Darks::UI {
	AutonomousModeWindow::AutonomousModeWindow(
		AutonomousWorker& auto_worker,
		Darks::Controllers::Servers::_2134::LootCrateFarmerController& loot_crate_farmer_controller,
		Darks::Controllers::Servers::_2134::VegetableFarmerController& vegetable_farmer_controller,
		Darks::Controllers::Servers::_2134::BerryFarmerController& berry_farmer_controller
	) :
		DarksImguiWindow("Autonomous Mode Window"),
		auto_worker_(auto_worker),
		loot_crate_farm_controller_(loot_crate_farmer_controller),
		vegetable_farmer_controller_(vegetable_farmer_controller),
		berry_farmer_controller_(berry_farmer_controller)
	{ }

	void AutonomousModeWindow::Update() {
		loot_crate_farm_controller_.DisplayCtrlPanel();
		vegetable_farmer_controller_.DisplayCtrlPanel();
		berry_farmer_controller_.DisplayCtrlPanel();

		auto_worker_.DisplayCtrlPanel();

		// Hide the display when requested.
		/*if (auto_worker_.DisplayCtrlPanel()) {
			this->IsVisible = false;
		}*/
	}
}
