#ifndef DARKS_CONTROLLERS_GARDEN_FLORAFARMERBASE_H_
#define DARKS_CONTROLLERS_GARDEN_FLORAFARMERBASE_H_

#include <string>

#include "../../SyncInfo.h"

#include "../IQueueable.h"
#include "../SpawnController.h"
#include "../InventoryController.h"
#include "../MovementController.h"
#include "../CameraController.h"
#include "../KeyboardController.h"
#include "../MouseController.h"

namespace Darks::Controllers::Garden {
	class FloraFarmer {
	public:
		FloraFarmer(
			SpawnController& spawn_controller,
			InventoryController& inventory_controller,
			MovementController& movement_controller,
			CameraController& camera_controller
		);

	protected:
		const std::string CROP_COLUMN_BED_NAME = "CROP COLUMN";

		int timer_id_ = 0;

		KeyboardController keyboard_controller_{};
		MouseController mouse_controller_{};
		SpawnController& spawn_controller_;
		InventoryController& inventory_controller_;
		MovementController& movement_controller_;
		CameraController& camera_controller_;

		/// <summary>
		/// Routes the character to the correct fridge line. Call this function after dismounting the last chair before traversal to the correct fridge line is needed.
		/// </summary>
		/// <param name="crop"></param>
		void WalkToCropFridge(SyncInfo& info, int fridge_index) const;

		void HarvestCrops(SyncInfo& info, int iterations, std::string crop_search_str) const;

		bool SpawnAtCropBed(SyncInfo& info, int index) const {
			return spawn_controller_.Spawn(info, std::format("{} {}", CROP_COLUMN_BED_NAME, index + 1));
		}
		
		/// <summary>
		/// Routes the character to the last chair before routing to fridges is needed. Call this function after harvesting all desired crops.
		/// </summary>
		/// <param name="info"></param>
		void TravelToFridges(SyncInfo& info) const;

		/// <summary>
		/// Routes the character down the fridge deposit line depositing berries along the way. Call this function after pathing to the correct line of fridges.
		/// </summary>
		/// <param name="info"></param>
		/// <param name="crop_type"></param>
		/// <param name="fridge_iterations"></param>
		void DepositCropsInFridges(SyncInfo& info, int crop_type, int fridge_iterations) const;
	};
}

#endif