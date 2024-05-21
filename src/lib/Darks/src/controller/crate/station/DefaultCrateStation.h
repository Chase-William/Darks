#ifndef DARKS_CONTROLLER_CRATE_STATION_DEFAULTCRATESTATION_H_
#define DARKS_CONTROLLER_CRATE_STATION_DEFAULTCRATESTATION_H_

#include <string>
#include <memory>

#include "../Crate.h"
#include "../navigator/INavigateable.h"
#include "CrateStation.h"
#include "../../../io/Pixel.h"
#include "../../InventoryController.h"
#include "../../CameraController.h"
#include "../../GeneralController.h"
#include "../../SpawnController.h"
#include "../../SuicideController.h"

namespace Darks::Controller::Crate::Station {
	class DefaultCrateStation : public CrateStation {
	public:
		DefaultCrateStation(
			CrateConfig& crate,
			InventoryController& inventory_controller,
			CameraController& camera_controller,
			GeneralController& general_controller,
			SpawnController& spawn_controller,
			SuicideController& suicide_controller,
			std::optional<std::function<void(std::unique_ptr<CrateMessage> msg)>> on_crate_msg = std::nullopt,
			std::unique_ptr<Navigator::INavigateable> to_crate_navigator = nullptr
		) : CrateStation(
				crate, 
				inventory_controller, 
				camera_controller, 
				general_controller, 
				spawn_controller, 
				suicide_controller,
				on_crate_msg,
				std::move(to_crate_navigator))
		{ }

		void Execute(SyncInfo& info) const override;

	private:
		/// <summary>
		/// The maximum attempts permitted when attempting to loot a crate.
		/// </summary>
		const int ALLOWED_LOOT_ATTEMPTS = 5;
	};
}

#endif