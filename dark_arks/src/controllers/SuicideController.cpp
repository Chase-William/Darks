#include "SuicideController.h"

SuicideController::SuicideController(
	SuicideConfig conf,
	InventoryController& invent_controller,
	KeyboardController& keyboard_controller,
	MouseController& mouse_controller
) :
	conf_(conf),
	invent_controller_(invent_controller),
	keyboard_controller_(keyboard_controller),
	mouse_controller_(mouse_controller)
{ }

void SuicideController::Suicide(TaskInfo info) {
	DARKS_TRACE("Requested suicide.");

	// Check if the inventory is open as we need it to suicide
	if (!invent_controller_.IsOpen(Inventory::Self)) {
		DARKS_TRACE("Inventory not already open.");
		if (!invent_controller_.Open(Inventory::Self, TaskInfo(info.shutdown, info.cond_var, info.mut, 5000, 500))) {
			DARKS_ERROR("Failed to open inventory so that the character may suicide.");
			throw std::runtime_error("Unable to open inventory to suicide.");
		}
	}

	// Mouse cursor over implant and focus it
	mouse_controller_.Click(conf_.implant_pos_);
	/*std::this_thread::sleep_for(std::chrono::milliseconds(500));
	mouse_controller_.Click();*/
	// If unable to perform click handle

	// CURSOR ON TOP OF IMPLANT

	// Wait for implant suicide cooldown timer
	std::unique_lock<std::mutex> lock(info.mut);
	info.cond_var.wait_for(lock, std::chrono::milliseconds(conf_.implant_ready_delay_));
	if (info.shutdown) // Handle user wanting manual shutdown
	{
		DARKS_INFO("Initiating autonomous manual shutdown.");
		throw ManualShutdownException();
	}

	// Ready for the blissful release of death
	// Presses E on implant to commit
	keyboard_controller_.Keystroke(Key::E);

	//
	//
	//	Check if suicide was successful?
	//
	//

	DARKS_INFO("Suicide Successful.");
}

// - Need a better way to return specific errors, maybe use std::expect<T, E> like Result<T, E> from Rust lang
// - Should make a custom exception for when users are manually shutting down autonomous mode
// - Need some type of logging so that we can get insite into how things are running 