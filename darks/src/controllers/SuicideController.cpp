#include "SuicideController.h"

namespace Darks::Controllers {
	SuicideController::SuicideController(
		SuicideConfig conf,
		SpawnController& spawn_controller,
		InventoryController& inventory_controller
	) :
		conf_(conf),
		spawn_controller_(spawn_controller),
		inventory_controller_(inventory_controller)
	{ }

	bool SuicideController::Suicide(
		SyncInfo& info,
		int wait_for_inventory_open_poll_interval,
		int wait_for_inventory_open_timeout,
		int wait_for_disappearing_spawn_screen_poll_interval,
		int wait_for_disappearing_spawn_screen_timeout
	) const {
		DARKS_TRACE("Requested suicide.");

		// If the character is already dead, no need to suicide, or objective already complete
		if (spawn_controller_.IsSpawnScreenOpen(SpawnScreen::DeathScreen)) {
			DARKS_INFO("Character already dead.");
			return true;
		}

		// Check if the inventory is open as we need it to suicide
		if (!inventory_controller_.IsOpen(Inventory::Self)) {
			DARKS_TRACE("Inventory not already open.");
			if (!inventory_controller_.Open(info, Inventory::Self, wait_for_inventory_open_poll_interval, wait_for_inventory_open_timeout, 500)) {
				auto msg = "Unable to open inventory when attempting to suicide.";
				DARKS_ERROR(msg);
				throw AutonomousResetRequiredException(msg);
			}
		}

		// Mouse cursor over implant and focus it
		mouse_controller_.Click(conf_.implant_pos_);

		// CURSOR ON TOP OF IMPLANT

		// Wait for implant suicide cooldown timer
		info.Wait(conf_.implant_ready_delay_);

		// Ready for the blissful release of death
		// Presses E on implant to commit
		keyboard_controller_.Keystroke(Darks::IO::Key::E);
		// Check to see if the inventory disappears
		auto status = info.Wait(*this, wait_for_disappearing_spawn_screen_poll_interval, wait_for_disappearing_spawn_screen_timeout);
		DARKS_INFO(std::format("Suicide was {}.", (status ? "Successful" : "Unsuccessful")));
		return status;
	}

	bool SuicideController::Check(int code) const {
		// When suiciding, the player will always be routed to the death screen.
		// Therefore, only and always check for the disappearance of the death screen.
		return !inventory_controller_.IsOpen(Inventory::Self);
	}
}