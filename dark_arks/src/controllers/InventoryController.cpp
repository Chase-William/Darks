#include "InventoryController.h"

InventoryController::InventoryController(InventoryConfig conf) :
	conf_(conf)
{ }

bool InventoryController::Open(
	SyncInfo & info, 
	Inventory target, 
	int wait_for_inventory_open_poll_interval, 
	int wait_for_inventory_open_timeout, 
	int after_success_delay_
) const {
	DARKS_TRACE(fmt::format("Attempting to open {} inventory.", ToString(target)));

	// Inventory already open
	if (IsOpen(target)) {
		DARKS_TRACE("Inventory already open.");
		return true;
	}

	// Send open invent key in-game
	SendInventoryUpdateKey(target);

	// Check if inventory opens
	if (info.Wait(*this, wait_for_inventory_open_poll_interval, wait_for_inventory_open_timeout, target)) {
		// Delay for interactability if successfully opened
		info.Wait(after_success_delay_);
		return true;
	}
	return false;
}

bool InventoryController::Close(
	SyncInfo& info, 
	Inventory target, 
	int wait_for_inventory_close_poll_interval, 
	int wait_for_inventory_close_timeout, 
	int after_success_delay_
) const {
	DARKS_TRACE(fmt::format("Attempting to close {} inventory.", ToString(target)));

	// Inventory already closed
	if (!IsOpen(target)) {
		DARKS_TRACE("Inventory already closed.");
		return true;
	}

	// Send close invent key in-game
	SendInventoryUpdateKey(target);

	// Check if inventoy closes
	if (!info.Wait(*this, wait_for_inventory_close_poll_interval, wait_for_inventory_close_timeout, target)) {
		// Will run if inventory has closed
		// Delay for interactability if succesfully closed
		info.Wait(after_success_delay_);
		return true;
	}
	return false;
}

bool InventoryController::IsOpen(Inventory target) const {
	Color actual;
	switch (target) {
	case Inventory::Self:
		return Screen::GetPixelColor(conf_.self_inventory_open_pixel_.pos) == conf_.self_inventory_open_pixel_.color;
	case Inventory::Other:
		return Screen::GetPixelColor(conf_.other_inventory_open_pixel_.pos) == conf_.other_inventory_open_pixel_.color;
	default:
		throw new std::invalid_argument(fmt::format("Invalid argument of {:d} provided.", static_cast<int>(target)));
	}
}

bool InventoryController::Check(int code) const {
	return IsOpen(static_cast<Inventory>(code));
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

bool InventoryController::TransferAll(Inventory transfer_to) {
	// Both the self and other inventory must be open for a transfer to occur
	if (!IsOpen(Inventory::Other)) {
		DARKS_WARN("Cannot transfer all items, other inventory not open!");
		return false;
	}
	// If the other invent is open, then the self automatically is as well

	switch (transfer_to) {
	case Inventory::Self:
		// Click transfer all in other inventory to move all items into self
		mouse_controller_.Click(conf_.other_to_self_pixel_.pos);
		break;
	case Inventory::Other:
		// Click transfer all in self inventory to move all items into other
		mouse_controller_.Click(conf_.self_to_other_pixel_.pos);
		break;
	default:
		throw std::invalid_argument(fmt::format("The provided inventory argument of {:d} is invalid.", static_cast<int>(transfer_to)));
	}

	// If reached, operation successful
	return true;
}