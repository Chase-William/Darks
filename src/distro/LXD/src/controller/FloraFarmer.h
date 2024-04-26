#ifndef DARKS_CONTROLLERS_GARDEN_FLORAFARMERBASE_H_
#define DARKS_CONTROLLERS_GARDEN_FLORAFARMERBASE_H_

#include <string>

#include "Darks.h"

namespace LXD::Controller {
	class FloraFarmer {
	public:
		FloraFarmer(
			Darks::Controller::SpawnController& spawn_controller,
			Darks::Controller::InventoryController& inventory_controller,
			Darks::Controller::MovementController& movement_controller,
			Darks::Controller::CameraController& camera_controller
		);

	protected:
		const std::string CROP_COLUMN_BED_NAME = "CROP COLUMN";

		int timer_id_ = 0;

		Darks::Controller::KeyboardController keyboard_controller_{};
		Darks::Controller::MouseController mouse_controller_{};
		Darks::Controller::SpawnController& spawn_controller_;
		Darks::Controller::InventoryController& inventory_controller_;
		Darks::Controller::MovementController& movement_controller_;
		Darks::Controller::CameraController& camera_controller_;

		/// <summary>
		/// Routes the character to the correct fridge line. Call this function after dismounting the last chair before traversal to the correct fridge line is needed.
		/// </summary>
		/// <param name="crop"></param>
		void WalkToCropFridge(Darks::SyncInfo& info, int fridge_index) const;

		void HarvestCrops(Darks::SyncInfo& info, int iterations, std::string crop_search_str) const;

		bool SpawnAtCropBed(Darks::SyncInfo& info, int index) const {
			return spawn_controller_.Spawn(info, std::format("{} {}", CROP_COLUMN_BED_NAME, index + 1));
		}
		
		/// <summary>
		/// Routes the character to the last chair before routing to fridges is needed. Call this function after harvesting all desired crops.
		/// </summary>
		/// <param name="info"></param>
		void TravelToFridges(Darks::SyncInfo& info) const;

		/// <summary>
		/// Routes the character down the fridge deposit line depositing berries along the way. Call this function after pathing to the correct line of fridges.
		/// </summary>
		/// <param name="info"></param>
		/// <param name="crop_type"></param>
		/// <param name="fridge_iterations"></param>
		void DepositCropsInFridges(Darks::SyncInfo& info, int crop_type, int fridge_iterations) const;
	};
}

#endif