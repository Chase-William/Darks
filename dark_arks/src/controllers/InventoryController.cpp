#include "InventoryController.h"

InventoryController::InventoryController(InventoryConfig conf, KeyboardController& keyboard_controller) :
	conf_(conf),
	keyboard_controller_(keyboard_controller)
{ }

bool InventoryController::IsOpen(Inventory target) const {
	auto actual = Screen::GetPixelColor(conf_.inventory_open_pixel_.pos);
	auto expected = conf_.inventory_open_pixel_.color;
	// Get the pixel color	
	return Screen::GetPixelColor(conf_.inventory_open_pixel_.pos) == conf_.inventory_open_pixel_.color;
}

bool InventoryController::Open(Inventory target, TaskInfo info) const {
	DARKS_TRACE(fmt::format("Attempting to open {} inventory.", ToString(target)));

	// Inventory already open
	if (IsOpen(target)) {
		DARKS_TRACE("Inventory already open.");
		return true;
	}

	// Send open invent key in-game
	SendInventoryUpdateKey(target);
	
	return CheckInventoryUntilTimeout(target, info, [this](Inventory target) { return IsOpen(target); });
}

bool InventoryController::Close(Inventory target, TaskInfo info) const {
	DARKS_TRACE(fmt::format("Attempting to close {} inventory.", ToString(target)));

	// Inventory already closed
	if (!IsOpen(target)) {
		DARKS_TRACE("Inventory already closed.");
		return true;
	}

	// Send close invent key in-game
	SendInventoryUpdateKey(target);

	return CheckInventoryUntilTimeout(target, info, [this](Inventory target) { return !IsOpen(target); });
}

void InventoryController::SendInventoryUpdateKey(Inventory target) const {
	switch (target) {
	case Inventory::Self:
		keyboard_controller_.Keystroke(conf_.toggle_self_invent_key_);
		break;
	case Inventory::Other:
		keyboard_controller_.Keystroke(conf_.toggle_other_invent_key_);
		break;
	default:
		DARKS_CRITICAL("Invalid value for target inventory to open provided.");
		throw std::runtime_error("Invalid target given when attempting to open an inventory.");
	}
}

bool InventoryController::CheckInventoryUntilTimeout(Inventory target, TaskInfo info, std::function<bool(Inventory)> check) const
{
	// Check for updates at the provided interval while also checking for condtional variable update to shutdown current thread
	for (int accum_timeout = 0; accum_timeout < info.timeout; accum_timeout += info.interval) {
		std::unique_lock<std::mutex> lock(info.mut);
		info.cond_var.wait_for(lock, std::chrono::milliseconds(info.interval));

		if (info.shutdown) {
			DARKS_INFO("Shutdown initiated.");
			// User wants to terminate, should abort ASAP
			// This should be caught by callers and handled appropriately
			throw ManualShutdownException();
		}

		if (check(target)) {
			DARKS_TRACE(fmt::format("Inventory {} state successfully changed on iteration {:d}.", ToString(target), accum_timeout / info.interval));
			// Only return true if open, otherwise continue to next iteration
			return true;
		}
	}
	DARKS_TRACE(fmt::format("Inventory {} state change checker timedout, returning false as in failed.", ToString(target)));
	// Failed to open invent
	return false;
}