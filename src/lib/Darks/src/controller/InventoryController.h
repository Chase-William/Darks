#ifndef DARKS_CONTROLLERS_INVENTORYCONTROLLER_H_
#define DARKS_CONTROLLERS_INVENTORYCONTROLLER_H_

#include "cpr/cpr.h"

#include "../io/VirtualInput.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "../io/Pixel.h"
#include "../io/Screen.h"
#include "../SyncInfo.h"
#include "../ServiceContext.h"
#include "ILoadable.h"

namespace Darks::Controller {
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
	
	class InventoryConfig : public ILoadable {
	public:
		static const std::string URL_SUBDIRECTORY_NAME;

		IO::Key toggle_self_inventory_key_ = IO::Key::I;
		IO::Key toggle_other_inventory_key_ = IO::Key::F;

		/// <summary>
		/// The pixel to inspect to determine if an inventory is open or not.
		/// </summary>
		IO::Pixel self_inventory_open_pixel_ = IO::Pixel({ 0, 0 }, IO::Color(0, 0, 0));
		IO::Pixel other_inventory_open_pixel_ = IO::Pixel({ 0, 0 }, IO::Color(0, 0, 0));

		IO::Pixel self_to_other_pixel_ = IO::Pixel({ 0, 0 }, IO::Color(0, 0, 0));
		IO::Pixel other_to_self_pixel_ = IO::Pixel({ 0, 0 }, IO::Color(0, 0, 0));

		IO::Point focus_other_searchbar_pos_ = { 0, 0 };

		/*int tile_width_ = 100;
		int tile_height_ = 100;
		IO::Point tile_origin_ = IO::Point(375, 350);*/

		inline std::string GetUrl() const override {
			return std::string(GetServiceState().GetBaseUrl() + "/" + URL_SUBDIRECTORY_NAME);
		}
	};

	static void to_json(nlohmann::json& json, const InventoryConfig& conf) {
		json = nlohmann::json({
			{ "toggle_self_inventory_key", conf.toggle_self_inventory_key_ },
			{ "toggle_other_inventory_key", conf.toggle_other_inventory_key_ },
			{ "self_inventory_open_pixel", conf.self_inventory_open_pixel_ },
			{ "other_inventory_open_pixel", conf.other_inventory_open_pixel_ },
			{ "self_to_other_pixel", conf.self_to_other_pixel_ },
			{ "other_to_self_pixel", conf.other_to_self_pixel_ },
			{ "focus_other_searchbar_pos", conf.focus_other_searchbar_pos_ }
		});
	}

	static void from_json(const nlohmann::json& json, InventoryConfig& conf) {
		auto& machine = json.at("machine");		

		machine.at("toggle_self_inventory_key").get_to(conf.toggle_self_inventory_key_);
		machine.at("toggle_other_inventory_key").get_to(conf.toggle_other_inventory_key_);

		auto& resolution = json.at("resolution");

		resolution.at("self_inventory_open_pixel").get_to(conf.self_inventory_open_pixel_);
		resolution.at("other_inventory_open_pixel").get_to(conf.other_inventory_open_pixel_);
		resolution.at("self_to_other_pixel").get_to(conf.self_to_other_pixel_);
		resolution.at("other_to_self_pixel").get_to(conf.other_to_self_pixel_);
		resolution.at("focus_other_searchbar_pos").get_to(conf.focus_other_searchbar_pos_);
	}

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
		) const {
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
			mouse_controller_.Click(conf_.focus_other_searchbar_pos_);
			info.Wait(100); // Wait before typing
			keyboard_controller_.Keystrokes(query_str);
		}

		bool TransferAll(Inventory transfer_to);

		bool IsOpen(Inventory target) const;

		/// <summary>
		/// Returns a bool indicating if the inventory requested is empty.
		/// </summary>
		/// <param name="target">Inventory to insepect.</param>
		/// <param name="filtered">Whether the results shown are filtered via a searchbar, only applicable to self inventory.</param>
		/// <returns></returns>
		// bool IsEmpty(Inventory target, bool filtered = false) const;

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
}

#endif