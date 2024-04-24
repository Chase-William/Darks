#ifndef DARKS_CONTROLLERS_ISLAND_LOOTCRATES_H_
#define DARKS_CONTROLLERS_ISLAND_LOOTCRATES_H_

#include "../../../Log.h"
#include "../../../SyncInfo.h"
#include "../../IDisplayCtrlPanel.h"
#include "../../SpawnController.h"
#include "../../CameraController.h"
#include "../../InventoryController.h"
#include "../../SuicideController.h"
#include "../../CameraController.h"
#include "../../GeneralController.h"
#include "../../MovementController.h"

namespace Darks::Controllers::Servers::_2134 {	

	/// <summary>
	/// The base class for all crates.
	/// </summary>
	class LootCrateInfo {
	public:
		LootCrateInfo(
			std::string bed_name,
			IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			InventoryController& inventory_controller,
			CameraController& camera_controller,
			GeneralController& general_controller,
			SpawnController& spawn_controller,
			SuicideController& suicide_controller,
			std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate
		) : 
			bed_name_(bed_name),
			crate_rect_(crate_rect),
			enabled_(enabled),
			is_double_harvestable_(is_double_harvestable),
			inventory_controller_(inventory_controller),
			camera_controller_(camera_controller),
			general_controller_(general_controller),
			spawn_controller_(spawn_controller),
			suicide_controller_(suicide_controller),
			on_crate_(on_crate)
		{ }

		virtual void Execute(SyncInfo& info) = 0;		

		std::string GetBedName() const { return bed_name_; };
		bool IsDoubleHarvestable() const { return is_double_harvestable_; }

		bool* enabled_;		

	protected:	
		bool OpenCrate(SyncInfo& info) const {
			auto status = inventory_controller_.Open(info, Inventory::Other);
			if (!status) {
				DARKS_WARN("Was unable to open the loot crate.");			
			}
			return status;
		}
		void CloseCrate(SyncInfo& info) const {
			// Close crate inventory
			if (!inventory_controller_.Close(info, Inventory::Other)) {
				auto msg = "Was unable to close loot crate inventory!";
				DARKS_ERROR(msg);
				throw AutonomousResetRequiredException(msg);
			}
		}
		void CloseVault(SyncInfo& info) const {
			// Close vault inventory
			if (!inventory_controller_.Close(info, Inventory::Other)) {
				auto msg = "Was unable to close vault inventory!";
				DARKS_ERROR(msg);
				throw AutonomousResetRequiredException(msg);
			}
		}
		
		virtual void ExitCrate(SyncInfo& info) const {
			DARKS_TRACE("Exiting crate.");
			// Look down to bed
			camera_controller_.Pivot(info, CameraDirection::Down, 5000);
			// Use bed
			general_controller_.Use();

			// Check if bed spawn screen opened
			if (!info.Wait(spawn_controller_, 250, 5000, SpawnScreen::DeathScreen | SpawnScreen::FastTravelScreen)) {
				// If not opened, attempt suicide
				if (!suicide_controller_.Suicide(info)) {
					auto msg = "Was unable to suicide when leaving drop.";
					DARKS_ERROR(msg);
					throw AutonomousResetRequiredException(msg);
				}
			}

			// Allow time for ark to become properly interactable before proceeding
			info.Wait(5000);
		}

		/// <summary>
		/// Call to invoke the on_loot_crate_ event.
		/// </summary>
		inline void OnCrateOpened() const {
			std::unique_ptr<std::vector<char>> jpg_buf = IO::Screen::GetScreenshot(crate_rect_);
			// If it has been set, raise the loot crate event
			if (on_crate_)
				on_crate_(bed_name_, *jpg_buf);
		}

	protected:
		std::string bed_name_;
		IO::Rect crate_rect_;
		bool is_double_harvestable_;
		InventoryController& inventory_controller_;
		CameraController& camera_controller_;
		GeneralController& general_controller_;
		SpawnController& spawn_controller_;
		SuicideController& suicide_controller_;

		std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate_;
	};

	/// <summary>
	/// Use for the your standard crate setup.
	/// </summary>
	class DefaultCrate : public LootCrateInfo {
	public:
		DefaultCrate(
			std::string bed_name,
			IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			InventoryController& inventory_controller,
			CameraController& camera_controller,
			GeneralController& general_controller,
			SpawnController& spawn_controller,
			SuicideController& suicide_controller,
			std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate
		) : 
			LootCrateInfo(bed_name, crate_rect, enabled, is_double_harvestable, inventory_controller, camera_controller, general_controller, spawn_controller, suicide_controller, on_crate)
		{ }

		void Execute(SyncInfo& info) override {
			DARKS_TRACE(std::format("Attempting to loot crate: {}.", bed_name_));

			info.Wait(30000); // Wait for render
			
			// Begin attempting to access the drop's inventory
			for (int i = 0; i < ALLOWED_LOOT_ATTEMPTS; i++) {
				if (!inventory_controller_.Open(info, Inventory::Other)) {
					// Failed to open other inventory, so look down a little and try again
					camera_controller_.Pivot(info, CameraDirection::Down, 150);
				}
				else break; // break out when successfully opened
			}

			// Ensure drops inventory is open
			if (!inventory_controller_.IsOpen(Inventory::Other)) {
				// Failed to open the drop inventory, exit crate
				ExitCrate(info);
				return;
			}

			OnCrateOpened();

			// Take all from crate
			inventory_controller_.TransferAll(Inventory::Self);

			CloseCrate(info);

			// Look up to vault to deposit
			camera_controller_.Pivot(info, CameraDirection::Up, 5000); // Lookup for 5 seconds
			// Access vault inventory
			if (!inventory_controller_.Open(info, Inventory::Other)) {
				// Failed to access vault, exit crate
				DARKS_WARN("Was unable to deposit crate items!");
				ExitCrate(info);
				return;
			}
			// Give all to vault
			inventory_controller_.TransferAll(Inventory::Other);
			
			CloseVault(info);
			
			// Exit crate, successful
			ExitCrate(info);

			DARKS_TRACE(std::format("Completed looting & exiting crate: {}.", bed_name_));
		}	
	private:
		/// <summary>
		/// The maximum attempts permitted when attempting to loot a crate.
		/// </summary>
		const int ALLOWED_LOOT_ATTEMPTS = 5;
	};

	/// <summary>
	/// Use for swamp cave crate setups.
	/// </summary>
	class SwampCaveCrate : public LootCrateInfo {
	public:
		SwampCaveCrate(
			std::string bed_name,
			IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			InventoryController& inventory_controller,
			CameraController& camera_controller,
			GeneralController& general_controller,
			SpawnController& spawn_controller,
			SuicideController& suicide_controller,
			std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate
		) :
			LootCrateInfo(bed_name, crate_rect, enabled, is_double_harvestable, inventory_controller, camera_controller, general_controller, spawn_controller, suicide_controller, on_crate)
		{ }

		void Execute(SyncInfo& info) override {
			DARKS_TRACE(std::format("Attempting to loot crate: {}.", bed_name_));

			info.Wait(15000); // Wait for render

			// Begin attempting to access the drop's inventory
			for (int i = 0; i < ALLOWED_LOOT_ATTEMPTS; i++) {
				if (!inventory_controller_.Open(info, Inventory::Other)) {
					// Failed to open other inventory, so look down a little and try again
					camera_controller_.Pivot(info, CameraDirection::Down, 100);
				}
			}

			// Ensure drops inventory is open
			if (!inventory_controller_.IsOpen(Inventory::Other)) {
				// Failed to open the drop inventory, exit crate
				ExitCrate(info);
				return;
			}

			OnCrateOpened();

			// Take all from crate
			inventory_controller_.TransferAll(Inventory::Self);

			CloseCrate(info);

			// Look up to vault to deposit
			camera_controller_.Pivot(info, CameraDirection::Up, 5000); // Lookup for 5 seconds
			// Access vault inventory
			if (!inventory_controller_.Open(info, Inventory::Other)) {
				// Failed to access vault, exit crate
				DARKS_WARN("Was unable to deposit crate items!");
				ExitCrate(info);
				return;
			}
			// Give all to vault
			inventory_controller_.TransferAll(Inventory::Other);

			CloseVault(info);

			// Exit crate, successful
			ExitCrate(info);

			DARKS_TRACE(std::format("Completed looting & exiting crate: {}.", bed_name_));
		}

	protected:
		void ExitCrate(SyncInfo& info) const override {
			DARKS_TRACE("Exiting crate.");

			// Wait for character to die to swamp cave's poisonous atmosphere
			if (!info.Wait(spawn_controller_, 500, 60000, SpawnScreen::DeathScreen | SpawnScreen::FastTravelScreen)) {
				// If the character is failing to die (maybe spawned elsewher), commit suicide
				if (!suicide_controller_.Suicide(info)) {
					auto msg = "Was unable to suicide when leaving drop.";
					DARKS_ERROR(msg);
					throw AutonomousResetRequiredException(msg);
				}
			}

			// Allow time for ark to become properly interactable before proceeding
			info.Wait(5000);
		}

	private:
		/// <summary>
		/// The maximum attempts permitted when attempting to loot a crate.
		/// </summary>
		const int ALLOWED_LOOT_ATTEMPTS = 3;
	};

	/// <summary>
	/// Use for skylord crate setup.
	/// </summary>
	class SkylordCaveCrate : public LootCrateInfo {
	public:
		SkylordCaveCrate(
			std::string bed_name,
			IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			InventoryController& inventory_controller,
			CameraController& camera_controller,
			GeneralController& general_controller,
			SpawnController& spawn_controller,
			SuicideController& suicide_controller,
			std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate
		) :
			LootCrateInfo(bed_name, crate_rect, enabled, is_double_harvestable, inventory_controller, camera_controller, general_controller, spawn_controller, suicide_controller, on_crate)
		{ }

		void Execute(SyncInfo& info) override {
			DARKS_TRACE(std::format("Attempting to loot crate: {}.", bed_name_));

			info.Wait(25000); // Wait for render

			if (!OpenCrate(info)) {
				// If failed to open crate, exit
				ExitCrate(info);
				return;
			}

			OnCrateOpened();

			// Take all from crate
			inventory_controller_.TransferAll(Inventory::Self);

			CloseCrate(info);

			// Begin attempting to access the vaults inventory 
			camera_controller_.Pivot(info, CameraDirection::Left, 1000); // Look left for 500 initially
			for (int i = 0; i < ALLOWED_DEPOSIT_ATTEMPTS_; i++) {
				if (!inventory_controller_.Open(info, Inventory::Other)) {
					// Failed to open vault, look left a little more
					camera_controller_.Pivot(info, CameraDirection::Left, 300);
				}
			}

			// Ensure vault is open, otherwise exit
			if (!inventory_controller_.IsOpen(Inventory::Other)) {
				// Failed to open the drop inventory, exit crate
				DARKS_WARN("Failed to open crate.");
				ExitCrate(info);
				return;
			}

			// Deposit loot in vault
			inventory_controller_.TransferAll(Inventory::Other);
			
			CloseVault(info);

			// Exit crate, successful
			ExitCrate(info);

			DARKS_TRACE(std::format("Completed looting & exiting crate: {}.", bed_name_));
		}
	private:
		const int ALLOWED_DEPOSIT_ATTEMPTS_ = 15;
	};

	/// <summary>
	///	Use for both pack cave crate setups.
	/// </summary>
	class PackCaveCrate : public LootCrateInfo {
	public:
		PackCaveCrate(
			std::string bed_name,
			IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			InventoryController& inventory_controller,
			CameraController& camera_controller,
			GeneralController& general_controller,
			SpawnController& spawn_controller,
			SuicideController& suicide_controller,
			MovementController& movement_controller,
			std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate
		) : 
			LootCrateInfo(bed_name, crate_rect, enabled, is_double_harvestable, inventory_controller, camera_controller, general_controller, spawn_controller, suicide_controller, on_crate),
			movement_controller_(movement_controller)
		{ }

		void Execute(SyncInfo& info) override {
			DARKS_TRACE(std::format("Attempting to loot crate: {}.", bed_name_));

			info.Wait(25000); // Wait for render

			// Mount chair
			general_controller_.Use();
			// Wait a moment before dismount
			info.Wait(2000);
			// Dismount chair to fall into water below
			general_controller_.Use();
			// Wait a moment so chair dismount can finish adjusting camera before we do
			info.Wait(2000);
			// Look down as drop will be below player
			camera_controller_.Pivot(info, CameraDirection::Down, 5000);
			// Travel downwards into water to drop
			movement_controller_.Descend(info, 10000);
			// Open the drop
			if (!OpenCrate(info)) {
				DARKS_TRACE("The crate was missing or unaccessible.");
				// Failed to open crate, exit
				ExitCrate(info);
				return;
			}

			OnCrateOpened();

			// Take all items
			inventory_controller_.TransferAll(Inventory::Self);
			// Close the crate
			
			if (!inventory_controller_.Close(info, Inventory::Other)) {
				// Failed to close drop
				DARKS_WARN("Failed to close crate inventory.");
				ExitCrate(info);
				return;
			}

			// Look up to vault for depo
			camera_controller_.Pivot(info, CameraDirection::Up, 750); // Initial look up
			for (int i = 0; i < ALLOWED_LOOT_ATTEMPTS_; i++) {
				if (!inventory_controller_.Open(info, Inventory::Other)) {
					// If it fails to open, look up a little more
					camera_controller_.Pivot(info, CameraDirection::Up, 150);
				}
			}
			// Check if vault did finally open
			if (!inventory_controller_.IsOpen(Inventory::Other)) {
				// Failed to open vault, exit
				DARKS_WARN("Failed to open vault.");
				ExitCrate(info);
				return;
			}
			// Give all items
			inventory_controller_.TransferAll(Inventory::Other);
			// Close Inventory
			if (!inventory_controller_.Close(info, Inventory::Other)) {
				// Failed to close the vault
				DARKS_WARN("Failed to close vault.");
				ExitCrate(info);
				return;
			}

			ExitCrate(info);
			DARKS_TRACE(std::format("Completed looting & exiting crate: {}.", bed_name_));
		}
	protected:
		void ExitCrate(SyncInfo& info) const override {
			// Failed to open crate, suicide
			if (!suicide_controller_.Suicide(info)) {
				// Failed to suicide, maybe the character is already dead?
				if (!spawn_controller_.IsSpawnScreenOpen(SpawnScreen::DeathScreen)) {
					auto msg = "Was unable to suicide.";
					DARKS_ERROR(msg);
					throw AutonomousResetRequiredException(msg);
					return;
				}
				// Spawn screen is open, just return below
			}

			// Allow time for ark to become properly interactable before proceeding
			info.Wait(5000);
		}

	protected:
		MovementController& movement_controller_;

	private:
		const int ALLOWED_LOOT_ATTEMPTS_ = 5;
	};
}

#endif