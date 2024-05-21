#ifndef DARKS_CONTROLLER_CRATE_STATION_CRATESTATION_H_
#define DARKS_CONTROLLER_CRATE_STATION_CRATESTATION_H_

#include <functional>
#include <memory>
#include <string>

#include "../Crate.h"
#include "../navigator/INavigateable.h"
#include "../../InventoryController.h"
#include "../../CameraController.h"
#include "../../GeneralController.h"
#include "../../SpawnController.h"
#include "../../SuicideController.h"

namespace Darks::Controller::Crate::Station {	
	class CrateStation {
	public:
		CrateStation(
			CrateConfig& crate,
			InventoryController& inventory_controller,
			CameraController& camera_controller,
			GeneralController& general_controller,
			SpawnController& spawn_controller,
			SuicideController& suicide_controller,
			std::optional<std::function<void(std::unique_ptr<CrateMessage> msg)>> on_crate_msg = std::nullopt,
			std::unique_ptr<Navigator::INavigateable> to_crate_navigator = nullptr
		);

		virtual void Execute(SyncInfo& info) const = 0;

		inline bool IsEnabled() const { return conf_.enabled_; }
		inline bool IsDoubleHarvestable() const { return conf_.is_double_harvestable_; }
		inline std::string GetBedName() const { return conf_.bed_name_; }

	protected:

		void Harvest(SyncInfo& info) const;

		void Deposit(SyncInfo& info) const;

		/// <summary>
		/// Open a crate the player is already looking at.
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		inline bool OpenCrate(SyncInfo& info) const {
			auto status = inventory_controller_.Open(info, Inventory::Other);			
			if (status)
				DARKS_INFO(std::format("Successfully opened loot crate {}'s inventory.", conf_.bed_name_));			
			else
				DARKS_INFO(std::format("Was unable to open loot crate {}.", conf_.bed_name_));			
			return status;
		}

		inline void CloseCrate(SyncInfo& info) const {
			// Close crate inventory
			if (!inventory_controller_.Close(info, Other)) {
				auto msg = std::format("Was unable to close loot crate {}'s inventory!", conf_.bed_name_);
				DARKS_WARN(msg);
				throw AutonomousResetRequiredException(msg.c_str());
			}
			else DARKS_INFO(std::format("Successfully opened crate {}.", conf_.bed_name_));
		}

		inline bool OpenDepositContainerInventory(SyncInfo& info) const {
			auto status = inventory_controller_.Open(info, Inventory::Other);
			if (status)
				DARKS_INFO(std::format("Successfully deposit container inventory for crate {}.", conf_.bed_name_));			
			else
				DARKS_INFO(std::format("Was unable to open deposit container inventory for crate station {}.", conf_.bed_name_));			
			return status;
		}

		inline void CloseDepositContainerInventory(SyncInfo& info) const {
			// Close vault inventory
			if (!inventory_controller_.Close(info, Inventory::Other)) {
				auto msg = std::format("Was unable to deposit container vault when farming loot crate {}.", conf_.bed_name_);
				DARKS_WARN(msg);
				throw AutonomousResetRequiredException(msg.c_str());
			}
			else DARKS_INFO(std::format("Successfully closed deposit container for crate {}.", conf_.bed_name_));
		}

		virtual void ExitCrateStation(SyncInfo& info) const {
			DARKS_INFO(std::format("Now exiting crate station {}.", conf_.bed_name_));

			if (conf_.wait_until_automatic_death_) {
				if (!info.Wait(suicide_controller_, 500, 60000, SpawnScreen::DeathScreen)) {
					DARKS_WARN("Failed to automatically die as supposed to according to the configuration set. Falling back on leaving using a bed.");
				}
				else {					
					DARKS_INFO("Successfully exited crate.");
					info.Wait(5000);
					return;
				}
			}

			// Look down to bed
			camera_controller_.Pivot(info, CameraDirection::Down, 5000);
			// Use bed
			general_controller_.Use();

			// Check if bed spawn screen opened
			if (!info.Wait(spawn_controller_, 250, 5000, SpawnScreen::DeathScreen | SpawnScreen::FastTravelScreen)) {
				// If not opened, attempt suicide
				if (!suicide_controller_.Suicide(info)) {
					auto msg = std::format("Was unable to suicided when exiting crate station {}!", conf_.bed_name_);
					DARKS_WARN(msg);
					throw AutonomousResetRequiredException(msg.c_str());
				}
			}

			DARKS_INFO("Successfully exited crate.");
			// Allow time for ark to become properly interactable before proceeding
			info.Wait(5000);
		}		

		CrateConfig& conf_;
		InventoryController& inventory_controller_;
		CameraController& camera_controller_;
		GeneralController& general_controller_;
		SpawnController& spawn_controller_;
		SuicideController& suicide_controller_;	

		std::optional<std::function<void(std::unique_ptr<CrateMessage> msg)>> on_crate_msg_;
		std::unique_ptr<Navigator::INavigateable> to_crate_navigator_type_;
	};
}

#endif