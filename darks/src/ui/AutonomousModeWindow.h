#ifndef DARKS_UI_AUTONOMOUSMODEWINDOW_H_
#define DARKS_UI_AUTONOMOUSMODEWINDOW_H_

#include "DarksImguiWindow.h"
#include "../AutonomousWorker.h"
#include "../controllers/IDisplayCtrlPanel.h"
#include "../controllers/servers/2134/LootCrateFarmerController.h"
#include "../controllers/servers/2134/VegetableFarmerController.h"
#include "../controllers/servers/2134/BerryFarmerController.h"

namespace Darks::UI {
	/// <summary>
	/// An imgui window for interacting with autonomous mode based options.
	/// </summary>
	class AutonomousModeWindow : public DarksImguiWindow {
	public:
		AutonomousModeWindow(
			AutonomousWorker& auto_worker,
			Darks::Controllers::Servers::_2134::LootCrateFarmerController& loot_crate_farmer_controller,
			Darks::Controllers::Servers::_2134::VegetableFarmerController& vegetable_farmer_controller,
			Darks::Controllers::Servers::_2134::BerryFarmerController& berry_farmer_controller
		);

		void Update() override;

	private:
		AutonomousWorker& auto_worker_;
		Darks::Controllers::Servers::_2134::LootCrateFarmerController& loot_crate_farm_controller_;
		Darks::Controllers::Servers::_2134::VegetableFarmerController& vegetable_farmer_controller_;
		Darks::Controllers::Servers::_2134::BerryFarmerController& berry_farmer_controller_;
	};
}

#endif