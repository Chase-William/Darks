#ifndef DARKS_CONTROLLERS_ISLAND_LOOTCRATES_H_
#define DARKS_CONTROLLERS_ISLAND_LOOTCRATES_H_

#include "Darks.h"

namespace LXD::Controller {

	/// <summary>
	/// The base class for all crates.
	/// </summary>
	class LootCrateInfo {
	public:
		LootCrateInfo(
			std::string bed_name,
			Darks::IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			Darks::Controller::InventoryController& inventory_controller,
			Darks::Controller::CameraController& camera_controller,
			Darks::Controller::GeneralController& general_controller,
			Darks::Controller::SpawnController& spawn_controller,
			Darks::Controller::SuicideController& suicide_controller,
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

		virtual void Execute(Darks::SyncInfo& info) = 0;		

		std::string GetBedName() const { return bed_name_; };
		bool IsDoubleHarvestable() const { return is_double_harvestable_; }

		bool* enabled_;		

	protected:	
		bool OpenCrate(Darks::SyncInfo& info) const {
			auto status = inventory_controller_.Open(info, Darks::Controller::Inventory::Other);
			if (!status) {
				DARKS_WARN("Was unable to open the loot crate.");			
			}
			return status;
		}
		void CloseCrate(Darks::SyncInfo& info) const {
			// Close crate inventory
			if (!inventory_controller_.Close(info, Darks::Controller::Inventory::Other)) {
				auto msg = "Was unable to close loot crate inventory!";
				DARKS_ERROR(msg);
				throw Darks::AutonomousResetRequiredException(msg);
			}
		}
		void CloseVault(Darks::SyncInfo& info) const {
			// Close vault inventory
			if (!inventory_controller_.Close(info, Darks::Controller::Inventory::Other)) {
				auto msg = "Was unable to close vault inventory!";
				DARKS_ERROR(msg);
				throw Darks::AutonomousResetRequiredException(msg);
			}
		}
		
		virtual void ExitCrate(Darks::SyncInfo& info) const {
			DARKS_TRACE("Exiting crate.");
			// Look down to bed
			camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Down, 5000);
			// Use bed
			general_controller_.Use();

			// Check if bed spawn screen opened
			if (!info.Wait(spawn_controller_, 250, 5000, Darks::Controller::SpawnScreen::DeathScreen | Darks::Controller::SpawnScreen::FastTravelScreen)) {
				// If not opened, attempt suicide
				if (!suicide_controller_.Suicide(info)) {
					auto msg = "Was unable to suicide when leaving drop.";
					DARKS_ERROR(msg);
					throw Darks::AutonomousResetRequiredException(msg);
				}
			}

			// Allow time for ark to become properly interactable before proceeding
			info.Wait(5000);
		}

		/// <summary>
		/// Call to invoke the on_loot_crate_ event.
		/// </summary>
		inline void OnCrateOpened() const {
			std::unique_ptr<std::vector<char>> jpg_buf = Darks::IO::Screen::GetScreenshot(crate_rect_);
			// If it has been set, raise the loot crate event
			if (on_crate_)
				on_crate_(bed_name_, *jpg_buf);
		}

	protected:
		std::string bed_name_;
		Darks::IO::Rect crate_rect_;
		bool is_double_harvestable_;
		Darks::Controller::InventoryController& inventory_controller_;
		Darks::Controller::CameraController& camera_controller_;
		Darks::Controller::GeneralController& general_controller_;
		Darks::Controller::SpawnController& spawn_controller_;
		Darks::Controller::SuicideController& suicide_controller_;

		std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate_;
	};

	/// <summary>
	/// Use for the your standard crate setup.
	/// </summary>
	class DefaultCrate : public LootCrateInfo {
	public:
		DefaultCrate(
			std::string bed_name,
			Darks::IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			Darks::Controller::InventoryController& inventory_controller,
			Darks::Controller::CameraController& camera_controller,
			Darks::Controller::GeneralController& general_controller,
			Darks::Controller::SpawnController& spawn_controller,
			Darks::Controller::SuicideController& suicide_controller,
			std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate
		) : 
			LootCrateInfo(bed_name, crate_rect, enabled, is_double_harvestable, inventory_controller, camera_controller, general_controller, spawn_controller, suicide_controller, on_crate)
		{ }

		void Execute(Darks::SyncInfo& info) override {
			DARKS_TRACE(std::format("Attempting to loot crate: {}.", bed_name_));

			info.Wait(30000); // Wait for render
			
			// Begin attempting to access the drop's inventory
			for (int i = 0; i < ALLOWED_LOOT_ATTEMPTS; i++) {
				if (!inventory_controller_.Open(info, Darks::Controller::Inventory::Other)) {
					if (!inventory_controller_.Open(info, Darks::Controller::Inventory::Other)) {
						// Failed to open other inventory, so look down a little and try again
						camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Down, 150);
					}
					else break; // break out when successfully opened
				}

				// Ensure drops inventory is open
				if (!inventory_controller_.IsOpen(Darks::Controller::Inventory::Other)) {
					// Failed to open the drop inventory, exit crate
					ExitCrate(info);
					return;
				}

				OnCrateOpened();

				// Take all from crate
				inventory_controller_.TransferAll(Darks::Controller::Inventory::Self);

				CloseCrate(info);

				// Look up to vault to deposit
				camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Up, 5000); // Lookup for 5 seconds
				// Access vault inventory
				if (!inventory_controller_.Open(info, Darks::Controller::Inventory::Other)) {
					// Failed to access vault, exit crate
					DARKS_WARN("Was unable to deposit crate items!");
					ExitCrate(info);
					return;
				}
				// Give all to vault
				inventory_controller_.TransferAll(Darks::Controller::Inventory::Other);

				CloseVault(info);

				// Exit crate, successful
				ExitCrate(info);

				DARKS_TRACE(std::format("Completed looting & exiting crate: {}.", bed_name_));
			}
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
			Darks::IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			Darks::Controller::InventoryController& inventory_controller,
			Darks::Controller::CameraController& camera_controller,
			Darks::Controller::GeneralController& general_controller,
			Darks::Controller::SpawnController& spawn_controller,
			Darks::Controller::SuicideController& suicide_controller,
			std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate
		) :
			LootCrateInfo(bed_name, crate_rect, enabled, is_double_harvestable, inventory_controller, camera_controller, general_controller, spawn_controller, suicide_controller, on_crate)
		{ }

		void Execute(Darks::SyncInfo& info) override {
			DARKS_TRACE(std::format("Attempting to loot crate: {}.", bed_name_));

			info.Wait(15000); // Wait for render

			// Begin attempting to access the drop's inventory
			for (int i = 0; i < ALLOWED_LOOT_ATTEMPTS; i++) {
				if (!inventory_controller_.Open(info, Darks::Controller::Inventory::Other)) {
					// Failed to open other inventory, so look down a little and try again
					camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Down, 100);
				}
			}

			// Ensure drops inventory is open
			if (!inventory_controller_.IsOpen(Darks::Controller::Inventory::Other)) {
				// Failed to open the drop inventory, exit crate
				ExitCrate(info);
				return;
			}

			OnCrateOpened();

			// Take all from crate
			inventory_controller_.TransferAll(Darks::Controller::Inventory::Self);

			CloseCrate(info);

			// Look up to vault to deposit
			camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Up, 5000); // Lookup for 5 seconds
			// Access vault inventory
			if (!inventory_controller_.Open(info, Darks::Controller::Inventory::Other)) {
				// Failed to access vault, exit crate
				DARKS_WARN("Was unable to deposit crate items!");
				ExitCrate(info);
				return;
			}
			// Give all to vault
			inventory_controller_.TransferAll(Darks::Controller::Inventory::Other);

			CloseVault(info);

			// Exit crate, successful
			ExitCrate(info);

			DARKS_TRACE(std::format("Completed looting & exiting crate: {}.", bed_name_));
		}

	protected:
		void ExitCrate(Darks::SyncInfo& info) const override {
			DARKS_TRACE("Exiting crate.");

			// Wait for character to die to swamp cave's poisonous atmosphere
			if (!info.Wait(spawn_controller_, 500, 60000, Darks::Controller::SpawnScreen::DeathScreen | Darks::Controller::SpawnScreen::FastTravelScreen)) {
				// If the character is failing to die (maybe spawned elsewher), commit suicide
				if (!suicide_controller_.Suicide(info)) {
					auto msg = "Was unable to suicide when leaving drop.";
					DARKS_ERROR(msg);
					throw Darks::AutonomousResetRequiredException(msg);
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
			Darks::IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			Darks::Controller::InventoryController& inventory_controller,
			Darks::Controller::CameraController& camera_controller,
			Darks::Controller::GeneralController& general_controller,
			Darks::Controller::SpawnController& spawn_controller,
			Darks::Controller::SuicideController& suicide_controller,
			std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate
		) :
			LootCrateInfo(bed_name, crate_rect, enabled, is_double_harvestable, inventory_controller, camera_controller, general_controller, spawn_controller, suicide_controller, on_crate)
		{ }

		void Execute(Darks::SyncInfo& info) override {
			DARKS_TRACE(std::format("Attempting to loot crate: {}.", bed_name_));

			info.Wait(25000); // Wait for render

			if (!OpenCrate(info)) {
				// If failed to open crate, exit
				ExitCrate(info);
				return;
			}

			OnCrateOpened();

			// Take all from crate
			inventory_controller_.TransferAll(Darks::Controller::Inventory::Self);

			CloseCrate(info);

			// Begin attempting to access the vaults inventory 
			camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Left, 1000); // Look left for 500 initially
			for (int i = 0; i < ALLOWED_DEPOSIT_ATTEMPTS_; i++) {
				if (!inventory_controller_.Open(info, Darks::Controller::Inventory::Other)) {
					// Failed to open vault, look left a little more
					camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Left, 300);
				}
			}

			// Ensure vault is open, otherwise exit
			if (!inventory_controller_.IsOpen(Darks::Controller::Inventory::Other)) {
				// Failed to open the drop inventory, exit crate
				DARKS_WARN("Failed to open crate.");
				ExitCrate(info);
				return;
			}

			// Deposit loot in vault
			inventory_controller_.TransferAll(Darks::Controller::Inventory::Other);
			
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
			Darks::IO::Rect crate_rect,
			bool* enabled,
			bool is_double_harvestable,
			Darks::Controller::InventoryController& inventory_controller,
			Darks::Controller::CameraController& camera_controller,
			Darks::Controller::GeneralController& general_controller,
			Darks::Controller::SpawnController& spawn_controller,
			Darks::Controller::SuicideController& suicide_controller,
			Darks::Controller::MovementController& movement_controller,
			std::function<void(const std::string&, std::vector<char>& jpg_buf)> on_crate
		) : 
			LootCrateInfo(bed_name, crate_rect, enabled, is_double_harvestable, inventory_controller, camera_controller, general_controller, spawn_controller, suicide_controller, on_crate),
			movement_controller_(movement_controller)
		{ }

		void Execute(Darks::SyncInfo& info) override {
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
			camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Down, 5000);
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
			inventory_controller_.TransferAll(Darks::Controller::Inventory::Self);
			// Close the crate
			
			if (!inventory_controller_.Close(info, Darks::Controller::Inventory::Other)) {
				// Failed to close drop
				DARKS_WARN("Failed to close crate inventory.");
				ExitCrate(info);
				return;
			}

			// Look up to vault for depo
			camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Up, 750); // Initial look up
			for (int i = 0; i < ALLOWED_LOOT_ATTEMPTS_; i++) {
				if (!inventory_controller_.Open(info, Darks::Controller::Inventory::Other)) {
					// If it fails to open, look up a little more
					camera_controller_.Pivot(info, Darks::Controller::CameraDirection::Up, 150);
				}
			}
			// Check if vault did finally open
			if (!inventory_controller_.IsOpen(Darks::Controller::Inventory::Other)) {
				// Failed to open vault, exit
				DARKS_WARN("Failed to open vault.");
				ExitCrate(info);
				return;
			}
			// Give all items
			inventory_controller_.TransferAll(Darks::Controller::Inventory::Other);
			// Close Inventory
			if (!inventory_controller_.Close(info, Darks::Controller::Inventory::Other)) {
				// Failed to close the vault
				DARKS_WARN("Failed to close vault.");
				ExitCrate(info);
				return;
			}

			ExitCrate(info);
			DARKS_TRACE(std::format("Completed looting & exiting crate: {}.", bed_name_));
		}
	protected:
		void ExitCrate(Darks::SyncInfo& info) const override {
			// Failed to open crate, suicide
			if (!suicide_controller_.Suicide(info)) {
				// Failed to suicide, maybe the character is already dead?
				if (!spawn_controller_.IsSpawnScreenOpen(Darks::Controller::SpawnScreen::DeathScreen)) {
					auto msg = "Was unable to suicide.";
					DARKS_ERROR(msg);
					throw Darks::AutonomousResetRequiredException(msg);
					return;
				}
				// Spawn screen is open, just return below
			}

			// Allow time for ark to become properly interactable before proceeding
			info.Wait(5000);
		}

	protected:
		Darks::Controller::MovementController& movement_controller_;

	private:
		const int ALLOWED_LOOT_ATTEMPTS_ = 5;
	};
}

#endif