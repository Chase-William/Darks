#include "CrateStation.h"

namespace Darks::Controller::Crate::Station {
	CrateStation::CrateStation(
		CrateConfig& crate_conf, 
		InventoryController& inventory_controller, 
		CameraController& camera_controller, 
		GeneralController& general_controller, 
		SpawnController& spawn_controller, 
		SuicideController& suicide_controller,
		std::optional<std::function<void(std::unique_ptr<CrateMessage> msg)>> on_crate_msg,
		std::unique_ptr<Navigator::INavigateable> to_crate_navigator
	) :
		conf_(crate_conf),
		inventory_controller_(inventory_controller),
		camera_controller_(camera_controller),
		general_controller_(general_controller),
		spawn_controller_(spawn_controller),
		suicide_controller_(suicide_controller),
		on_crate_msg_(on_crate_msg),
		to_crate_navigator_type_(std::move(to_crate_navigator))
	{ }

	void CrateStation::Harvest(SyncInfo & info) const {
		// Perform initial pivot if set
		if (conf_.acquire_crate_.initial_pivot_duration > 0)
			camera_controller_.Pivot(info, conf_.acquire_crate_.direction, conf_.acquire_crate_.initial_pivot_duration);
		// Skip to opening the crate if max attempts is not greather than 0
		if (conf_.acquire_crate_.max_pivot_step_count > 0)
			// Begin attempting to access the drop's inventory
			for (int i = 0; i < conf_.acquire_crate_.max_pivot_step_count; i++) {
				if (!OpenCrate(info)) {
					// Failed to open other inventory, pivot step again
					camera_controller_.Pivot(info, conf_.acquire_crate_.direction, conf_.acquire_crate_.pivot_step_duration);
				}
				else break;
			}
		else
			OpenCrate(info);

		// Afterwards, check if other inventory was finally opened
		if (!inventory_controller_.IsOpen(Inventory::Other)) {
			DARKS_INFO(std::format("Was unable to access crate {}. Maybe it wasn't off cooldown yet.", conf_.bed_name_));
			ExitCrateStation(info);
			return;
		}

		// Report crate has been successfully opened
		if (on_crate_msg_.has_value())
			on_crate_msg_.value()(std::make_unique<CrateMessage>(CrateMessageType::CrateOpened, conf_.bed_name_));

		// Take all from crate
		inventory_controller_.TransferAll(Darks::Controller::Inventory::Self);
		CloseCrate(info);
	}

	void CrateStation::Deposit(SyncInfo& info) const {
		// Look up to vault to deposit
		if (conf_.acquire_deposit_container_.initial_pivot_duration > 0)
			camera_controller_.Pivot(info, conf_.acquire_deposit_container_.direction, conf_.acquire_deposit_container_.initial_pivot_duration);

		// Skip to opening the deposit container if the max attempts is not greather than 0
		if (conf_.acquire_deposit_container_.max_pivot_step_count > 0)
			for (int i = 0; i < conf_.acquire_deposit_container_.max_pivot_step_count; i++) {
				if (!OpenDepositContainerInventory(info)) {
					// Failed to open deposit container inventory this iteration, continue to next
					camera_controller_.Pivot(info, conf_.acquire_deposit_container_.direction, conf_.acquire_deposit_container_.pivot_step_duration);
				}
				else break;
			}
		else
			OpenDepositContainerInventory(info);

		// Afterwards, check if other inventory was finally opened
		if (!inventory_controller_.IsOpen(Inventory::Other)) {
			auto msg = std::format("Was unable to access deposit container for crate {}. This could result in item loss.", conf_.bed_name_);
			DARKS_INFO(msg);
			//on_crate_msg_(std::make_unique<CrateMessage>(CrateMessageType::DepositContainerNotFound, conf_.bed_name_, msg));
			ExitCrateStation(info);
			return;
		}

		// Store items inside container
		inventory_controller_.TransferAll(Darks::Controller::Inventory::Other);
		CloseDepositContainerInventory(info);
	}
};