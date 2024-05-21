#ifndef DARKS_CONTROLLER_CRATE_STATION_CRATE_H_
#define DARKS_CONTROLLER_CRATE_STATION_CRATE_H_

#include <string>
#include <memory>
#include <vector>
#include <array>

#include "nlohmann/json.hpp"

#include "../CameraDirection.h"

namespace Darks::Controller::Crate {

	enum class CrateMessageType {
		CrateOpened = 199,
		Ok = 200,
		DepositContainerNotFound = 404
	};

	class CrateMessage {
	public:
		CrateMessage(
			CrateMessageType code,
			std::string bed_name,
			std::string description = ""
		);

		CrateMessageType code;
		std::string bed_name;
		std::string description;
		std::unique_ptr<std::vector<char>> crate_inventory_screenshot = nullptr;
	};

	/// <summary>
	/// An enum used to determine which kind of navigator to create.
	/// </summary>
	enum class CrateStationNavigatorType : int {
		None = 0,
		SwanDive
	};

	const std::array<std::string, 2> CRATE_STATION_NAVIGATOR_TYPES = { "None", "SwanDive" };

	enum class StationType {
		Default = 0
	};

	struct InventoryAcquisitionMovement {
	public:
		CameraDirection direction = CameraDirection::Left;
		int initial_pivot_duration = 0;
		int pivot_step_duration = 150;
		int max_pivot_step_count = 5;
	};

	/// <summary>
	/// Represents a single crate to be farmed.
	/// </summary>
	class CrateConfig {
	public:
		bool enabled_ = true;
		bool is_double_harvestable_ = false;
		bool wait_until_automatic_death_ = true;
		int load_delay_ = 15000;
		std::string bed_name_ = "";
		StationType station_type_ = StationType::Default;
		CrateStationNavigatorType to_crate_navigator_type_ = CrateStationNavigatorType::None;
		InventoryAcquisitionMovement acquire_crate_{};
		InventoryAcquisitionMovement acquire_deposit_container_{};
	};

	static void to_json(nlohmann::json& json, const InventoryAcquisitionMovement& conf) {
		json = nlohmann::json({
			{ "direction", conf.direction },
			{ "initial_pivot_duration", static_cast<int>(conf.initial_pivot_duration) },
			{ "pivot_step_duration", conf.pivot_step_duration },
			{ "max_pivot_step_count", conf.max_pivot_step_count }
		});
	}

	static void to_json(nlohmann::json& json, const CrateConfig& conf) {
		json = nlohmann::json({
			{ "enabled", conf.enabled_ },
			{ "is_double_harvestable", conf.is_double_harvestable_ },
			{ "wait_until_automatic_death", conf.wait_until_automatic_death_ },
			{ "load_delay", conf.load_delay_ },
			{ "bed_name", conf.bed_name_ },
			{ "station_type", static_cast<int>(conf.station_type_) },
			{ "to_crate_navigator_type", static_cast<int>(conf.to_crate_navigator_type_) },
			{ "acquire_crate", conf.acquire_crate_ },
			{ "acquire_deposit_container", conf.acquire_deposit_container_ }
		});
	}

	static void from_json(const nlohmann::json& json, InventoryAcquisitionMovement& conf) {
		json.at("direction").get_to(conf.direction);
		json.at("initial_pivot_duration").get_to(conf.initial_pivot_duration);
		json.at("pivot_step_duration").get_to(conf.pivot_step_duration);
		json.at("max_pivot_step_count").get_to(conf.max_pivot_step_count);
	}

	static void from_json(const nlohmann::json& json, CrateConfig& conf) {
		json.at("enabled").get_to(conf.enabled_);
		json.at("is_double_harvestable").get_to(conf.is_double_harvestable_);
		json.at("wait_until_automatic_death").get_to(conf.wait_until_automatic_death_);
		json.at("load_delay").get_to(conf.load_delay_);
		json.at("bed_name").get_to(conf.bed_name_);
		json.at("station_type").get_to(conf.station_type_);
		json.at("to_crate_navigator_type").get_to(conf.to_crate_navigator_type_);
		json.at("acquire_crate").get_to(conf.acquire_crate_);
		json.at("acquire_deposit_container").get_to(conf.acquire_deposit_container_);
	}
}

#endif