#ifndef DARKS_UI_AUTONOMOUSMODEWINDOW_H_
#define DARKS_UI_AUTONOMOUSMODEWINDOW_H_

#include "DarksImguiWindow.h"
#include "../AutonomousWorker.h"
#include "../controllers/IDisplayCtrlPanel.h"
#include "../controllers/island/LootCrateFarmerController.h"
#include "../controllers/garden/VegetableFarmerController.h"
#include "../controllers/garden/BerryFarmerController.h"

/// <summary>
/// An imgui window for interacting with autonomous mode based options.
/// </summary>
class AutonomousModeWindow : public DarksImguiWindow {
public:
	AutonomousModeWindow(
		AutonomousWorker& auto_worker,
		Darks::Controllers::Island::LootCrateFarmerController& loot_crate_farmer_controller,
		Darks::Controllers::Garden::VegetableFarmerController& vegetable_farmer_controller,
		Darks::Controllers::Garden::BerryFarmerController& berry_farmer_controller
	);

	void Update() override;	

private:
	AutonomousWorker& auto_worker_;
	Darks::Controllers::Island::LootCrateFarmerController& loot_crate_farm_controller_;
	Darks::Controllers::Garden::VegetableFarmerController& vegetable_farmer_controller_;
	Darks::Controllers::Garden::BerryFarmerController& berry_farmer_controller_;
};

#endif