#ifndef DARKS_CONTROLLERS_INVENTORYCONTROLLER_H_
#define DARKS_CONTROLLERS_INVENTORYCONTROLLER_H_

#include <functional>

#include "../Log.h"
#include "../io/VirtualInput.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "../io/Pixel.h"
#include "../io/Screen.h"
#include "../SyncInfo.h"

enum Inventory {
	/// <summary>
	/// Opens the player's inventory.
	/// </summary>
	Self = 0,
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
	Pixel self_inventory_open_pixel_ = Pixel({ 410, 195 }, Color(188, 244, 255));
	Pixel other_inventory_open_pixel_ = Pixel({ 1785, 195 }, Color(188, 244, 255));

	Pixel self_to_other_pixel_ = Pixel({ 490, 260 }, Color(0, 140, 171));	
	Pixel other_to_self_pixel_ = Pixel({ 1850, 260 }, Color(0, 140, 171));

	Point focus_other_searchbar_point = { 1680, 260 };
};

class InventoryController : public ICheckable {
public:
	InventoryController(InventoryConfig conf);
	
	/// <summary>
	/// Opens an inventory if not already opened.
	/// </summary>
	/// <param name="info">Sync information used for sleeping the thread.</param>
	/// <param name="target">The type of inventory to open.</param>
	/// <param name="wait_for_inventory_open_poll_interval">The poll interval to check for inventory open state updates.</param>
	/// <param name="wait_for_inventory_open_timeout">The max time permitted to pass when checking for inventory open state updates.</param>
	/// <param name="after_success_delay_">A delay used after successfully opening to allow the inventory more time to be property interactable before continueing.</param>
	/// <returns></returns>
	bool Open(
		SyncInfo& info, 
		Inventory target, 
		int wait_for_inventory_open_poll_interval, 
		int wait_for_inventory_open_timeout,
		int after_success_delay_
	) const;
	bool Open(
		SyncInfo& info,
		Inventory target
	) {
		return Open(info, target, 500, 10000, 1500);
	}

	/// <summary>
	/// Closes an inventory if not already closed.
	/// </summary>
	/// <param name="info">Sync information used for sleeping the thread.</param>
	/// <param name="target">The type of the inventory to close.</param>
	/// <param name="wait_for_inventory_close_poll_interval">The poll interval to check for inventory close state updates.</param>
	/// <param name="wait_for_inventory_close_timeout">The max time permitted to pass when checking for inventory close state updates.</param>
	/// <param name="after_success_delay_">A delay used after succesfully closing to allow more time for the new state to become property interactable.</param>
	/// <returns></returns>
	bool Close(
		SyncInfo& info,
		Inventory target,
		int wait_for_inventory_close_poll_interval,
		int wait_for_inventory_close_timeout,
		int after_success_delay_
	) const;
	bool Close(
		SyncInfo& info, 
		Inventory target
	) const {
		return Close(info, target, 500, 2000, 1500);
	}

	void Search(SyncInfo& info, std::string query_str) const {
		mouse_controller_.Click(conf_.focus_other_searchbar_point);
		info.Wait(100); // Wait before typing
		keyboard_controller_.Keystrokes(query_str);
	}

	bool TransferAll(Inventory transfer_to);

	bool IsOpen(Inventory target) const;

	bool Check(int code = 0) const override;

private:
	InventoryConfig conf_;
	MouseController mouse_controller_{};
	KeyboardController keyboard_controller_{};

	// bool CheckInventoryUntilTimeout(Inventory target, TaskInfo info, std::function<bool(Inventory)> check) const;

	/// <summary>
	/// Sends the key to toggle the in-game inventory state based on the give target.
	/// </summary>
	/// <param name="target">Type of inventory to toggle.</param>
	void SendInventoryUpdateKey(Inventory target) const;
};

#endif