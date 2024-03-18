#pragma once

#include <functional>

#include "../Log.h"
#include "../Exceptions.h"
#include "../io/VirtualInput.h"
#include "KeyboardController.h"
#include "../io/Pixel.h"
#include "../io/Screen.h"
#include "../TaskInfo.h"

enum class Inventory {
	/// <summary>
	/// Opens the player's inventory.
	/// </summary>
	Self,
	/// <summary>
	/// Opens an inventory other than the player's.
	/// </summary>
	Other
};

static std::string ToString(Inventory target) { return target == Inventory::Self ? "Self" : "Other"; }

class InventoryConfig {
public:
	Key toggle_self_invent_key_ = Key::R;
	Key toggle_other_invent_key_ = Key::F;
	
	/// <summary>
	/// The pixel to inspect to determine if an inventory is open or not.
	/// </summary>
	Pixel inventory_open_pixel_ = Pixel({ 410, 195 }, Color(188, 244, 255));
};

class InventoryController {
public:
	InventoryController(InventoryConfig conf, KeyboardController& keyboard_controller);
	
	/// <summary>
	/// Attempts to open the specified inventory.
	/// </summary>
	/// <param name="target">Which kind of inventory to attempt to open.</param>
	/// <returns>True if the inventory is open, false otherwise.</returns>
	bool Open(Inventory target, TaskInfo info) const;
	/// <summary>
	/// Attempts to close the specified inventory.
	/// </summary>
	/// <param name="target"></param>
	/// <returns>True if the inventory is closed, false otherwise.</returns>
	bool Close(Inventory target, TaskInfo info) const;

	bool IsOpen(Inventory target) const;

private:
	InventoryConfig conf_;
	KeyboardController& keyboard_controller_;

	bool CheckInventoryUntilTimeout(Inventory target, TaskInfo info, std::function<bool(Inventory)> check) const;

	/// <summary>
	/// Sends the key to toggle the in-game inventory state based on the give target.
	/// </summary>
	/// <param name="target">Type of inventory to toggle.</param>
	void SendInventoryUpdateKey(Inventory target) const;
};

