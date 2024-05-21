#ifndef DARKS_CONTROLLERS_SPAWNCONTROLLER_H_
#define DARKS_CONTROLLERS_SPAWNCONTROLLER_H_

#include <string>

#include "../io/Pixel.h"
#include "../io/Screen.h"
#include "MouseController.h"
#include "KeyboardController.h"
#include "../SyncInfo.h"
#include "ILoadable.h"

namespace Darks::Controller {
	enum SpawnScreen {
		FastTravelScreen = 0x001,
		DeathScreen = 0x002,
	};

	static SpawnScreen operator|(SpawnScreen v1, SpawnScreen v2) {
		return static_cast<SpawnScreen>(v1 + v2);
	}

	class SpawnConfig : public ILoadable {
	public:
		static const std::string URL_SUBDIRECTORY_NAME;

		// spawn button
		IO::Point spawn_btn_pos_ = { 0, 0 };

		// Searchbar
		IO::Point death_screen_searchbar_pos_ = { 0, 0 };
		IO::Point fast_travel_screen_searchbar_pos_ = { 0, 0 };

		// location of the top most bed from query results
		IO::Pixel select_bed_pixel_ = IO::Pixel({ 0, 0 }, IO::Color(0, 0, 0));
		// Using the "S" character in beds because they do not overlap, and are not white (the default load in bg color)
		IO::Pixel fast_travel_screen_open_pixel_ = IO::Pixel({ 0, 0 }, IO::Color(0, 0, 0));
		IO::Pixel death_screen_open_pixel_ = IO::Pixel({ 0, 0 }, IO::Color(0, 0, 0));

		inline std::string GetUrl() const override {
			return std::string(GetServiceState().GetBaseUrl() + "/" + URL_SUBDIRECTORY_NAME);
		}
	};

	static void to_json(nlohmann::json& json, const SpawnConfig& conf) {
		json = nlohmann::json({
			{ "spawn_btn_pos", conf.spawn_btn_pos_ },
			{ "death_screen_searchbar_pos", conf.death_screen_searchbar_pos_ },
			{ "fast_travel_screen_searchbar_pos", conf.fast_travel_screen_searchbar_pos_ },
			{ "select_bed_pixel", conf.select_bed_pixel_ },
			{ "fast_travel_screen_open_pixel", conf.fast_travel_screen_open_pixel_ },
			{ "death_screen_open_pixel", conf.death_screen_open_pixel_ }
		});
	}

	static void from_json(const nlohmann::json& json, SpawnConfig& conf) {
		json.at("spawn_btn_pos").get_to(conf.spawn_btn_pos_);
		json.at("death_screen_searchbar_pos").get_to(conf.death_screen_searchbar_pos_);
		json.at("fast_travel_screen_searchbar_pos").get_to(conf.fast_travel_screen_searchbar_pos_);
		json.at("select_bed_pixel").get_to(conf.select_bed_pixel_);
		json.at("fast_travel_screen_open_pixel").get_to(conf.fast_travel_screen_open_pixel_);
		json.at("death_screen_open_pixel").get_to(conf.death_screen_open_pixel_);
	}

	class SpawnController : public ICheckable {
	public:
		SpawnController(SpawnConfig conf);

		/// <summary>
		/// Spawns the player at a bed.
		/// </summary>
		/// <param name="info">Sync information to be used for sleeping the thread.</param>
		/// <param name="bed">The bed to be spawned at.</param>
		/// <param name="wait_for_spawn_screen_poll_interval">The poll interval used when checking if the spawn screen is present.</param>
		/// <param name="wait_for_spawn_screen_timeout">The max amount of time allowed to pass when checking if the spawn screen is present.</param>
		/// <param name="after_spawn_screen_visible">Delay used after spawn screen is initially visible to allow the UI more time to become properly interactable.</param>
		/// <param name="after_searchbar_focus_click">Delay after focusing the searchbar.</param>
		/// <param name="after_query_str_entered">Delay after entering a query string into the searchbar, often used to allow currently visibles beds on the map to update before proceeding.</param>
		/// <param name="after_bed_selection_click">Delay after selecting the bed to spawn on.</param>
		/// <param name="wait_for_disappearance_of_spawn_screen_poll_interval">The poll interval used when checking if the spawn screen has disappeared.</param>
		/// <param name="wait_for_disappearance_of_spawn_screen_timeout">The max amoutn of time allowed to pass when checking if the spawn screen has disappeared.</param>
		/// <returns>True if spawning was successful, false otherwise. Note, spawning is deemed successful even if the player has unknowingly spawned in the wrong location due to various reasons (etc. the bed was still on cooldown).</returns>
		bool Spawn(
			SyncInfo& info,
			std::string bed_name,
			int wait_for_spawn_screen_poll_interval,
			int wait_for_spawn_screen_timeout,
			int after_spawn_screen_visible,
			int after_searchbar_focus_click,
			int after_query_str_entered,
			int after_bed_selection_click,
			int wait_for_disappearance_of_spawn_screen_poll_interval,
			int wait_for_disappearance_of_spawn_screen_timeout
		) const;
		bool Spawn(SyncInfo& info, std::string bed_name) const {
			return Spawn(info, bed_name, 500, 10000, 5000, 2000, 2000, 5000, 500, 700);
		}

		bool Clear(SyncInfo& info) const;

		bool IsSpawnScreenOpen(SpawnScreen screen) const;

		bool Check(int code = 0) const override;

	private:
		SpawnConfig conf_;
		MouseController mouse_controller_{};
		KeyboardController keyboard_controller_{};

		bool IsDeathScreenOpen() const {
			return Darks::IO::Screen::GetPixelColor(conf_.death_screen_open_pixel_.pos) == conf_.death_screen_open_pixel_.color;
		}

		bool IsFastTravelScreenOpen() const {
			return Darks::IO::Screen::GetPixelColor(conf_.fast_travel_screen_open_pixel_.pos) == conf_.fast_travel_screen_open_pixel_.color;
		}

		bool IsBedSelected() const {
			return Darks::IO::Screen::GetPixelColor(conf_.select_bed_pixel_.pos) == conf_.select_bed_pixel_.color;
		}
	};
}

#endif