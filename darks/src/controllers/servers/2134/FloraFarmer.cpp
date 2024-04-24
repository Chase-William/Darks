#include "FloraFarmer.h"

namespace Darks::Controllers::Servers::_2134 {
	FloraFarmer::FloraFarmer(
		SpawnController& spawn_controller,
		InventoryController& inventory_controller,
		MovementController& movement_controller,
		CameraController& camera_controller
	) :
		spawn_controller_(spawn_controller),
		inventory_controller_(inventory_controller),
		movement_controller_(movement_controller),
		camera_controller_(camera_controller)
	{ }

	void FloraFarmer::WalkToCropFridge(SyncInfo& info, int fridge_index) const {

		// Iterate until crop index reached, meaning character standing in front of first fridge containing that crop type
		for (int i = 0; i < fridge_index; i++) {
			movement_controller_.Move(info, Movement::Forward, 3000);
			// Prevent the move left operation on the last iteration as the character
			// is already in front of the target fridge
			if (i < fridge_index - 1)
				movement_controller_.Move(info, Movement::Left, 2000);
		}
	}

	void FloraFarmer::HarvestCrops(SyncInfo& info, int iterations, std::string crop_search_str) const {
		// Iterate over crops harvesting
		for (int i = 0; i < iterations; i++) {
			// Move forward incrementally to harvest new crops
			movement_controller_.Move(info, Movement::Forward, 175);

			if (!inventory_controller_.Open(info, Inventory::Other)) {
				// If the inventory fails to open, go to the next iteration 
				// as maybe we are still too far away from the crops to begin with
				DARKS_TRACE("Failed to open crop plot inventory, moving foward a little.");
				continue;
			}
			// Search for our target crop
			inventory_controller_.Search(info, crop_search_str);
			// Give the game time to query the inventory
			info.Wait(250);
			// Take all crop
			inventory_controller_.TransferAll(Inventory::Self);
			// Close the inventory
			if (!inventory_controller_.Close(info, Inventory::Other)) {
				auto msg = std::format("Failed to close crop plot inventory on the {} iteration.", i);
				DARKS_ERROR(msg);
				throw AutonomousResetRequiredException(msg.c_str());
			}
		}
	}
	void FloraFarmer::TravelToFridges(SyncInfo& info) const {
		// Walk backwards all the way to the beds
		movement_controller_.Move(info, Movement::Backwards, 35000); // 20 seconds
		info.Wait(200);
		// Stop crouching
		movement_controller_.Crouch();
		info.Wait(500);
		// Walk left to the out teleporter
		movement_controller_.Move(info, Movement::Left, 7000);
		// Sit in the chair	
		keyboard_controller_.Keystroke(IO::Key::E);
		info.Wait(2000);
		// Dismount the chair
		keyboard_controller_.Keystroke(IO::Key::E);
		info.Wait(1500);
		// Look down at teleporter
		camera_controller_.Pivot(info, CameraDirection::Down, 3000);
		info.Wait(200);
		// Use default teleport
		keyboard_controller_.Keystroke(IO::Key::Z);
		info.Wait(10000);
		// Move to the right to align with chair
		movement_controller_.Move(info, Movement::Right, 1500);
		// Walk forward to chair
		movement_controller_.Move(info, Movement::Forward, 3000);
		// Mount chair
		keyboard_controller_.Keystroke(IO::Key::E);
		info.Wait(1500);
		// Dismount chair
		keyboard_controller_.Keystroke(IO::Key::E);
		// Walk to fridge
	}

	void FloraFarmer::DepositCropsInFridges(SyncInfo& info, int crop_type, int fridge_iterations) const {
		for (int fridge_index = 0; fridge_index < fridge_iterations; fridge_index++) {
			if (inventory_controller_.Open(info, Inventory::Other)) {
				inventory_controller_.TransferAll(Inventory::Other);
				if (!inventory_controller_.Close(info, Inventory::Other)) {
					auto msg = std::format("Unable to close refrigerator when depositing crops: {} on iteration: {}.", static_cast<int>(crop_type), fridge_index);
					DARKS_ERROR(msg);
					throw AutonomousResetRequiredException(msg.c_str());
				}
			}
			// Move right a bit even if a failure to access occured
			movement_controller_.Move(info, Movement::Right, 350);
		}

		// Look down to out bed
		camera_controller_.Pivot(info, CameraDirection::Down, 3000);
		keyboard_controller_.Keystroke(IO::Key::E);
	}
}