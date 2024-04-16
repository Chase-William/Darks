#ifndef DARKS_CONTROLLERS_SPAWNCONTROLLER_H_
#define DARKS_CONTROLLERS_SPAWNCONTROLLER_H_

#include <format>
#include <stdexcept>
#include <string>

#include "../Log.h"
#include "../io/Pixel.h"
#include "../io/Screen.h"
#include "MouseController.h"
#include "KeyboardController.h"
#include "../SyncInfo.h"

enum SpawnScreen {
	FastTravelScreen = 0x001,
	DeathScreen = 0x002,
};

static SpawnScreen operator|(SpawnScreen v1, SpawnScreen v2) {
	return static_cast<SpawnScreen>(v1 + v2);
}

class SpawnConfig {
public:
	// spawn button
	Point spawn_btn_pos = { 2220, 1280 };
	// location of the top most bed from query results
	Pixel bed_of_interest_pixel = Pixel({ 660, 270 }, Color(83, 39, 1));

	// Searchbar
	Point death_screen_searchbar_pos = { 270, 1285 };
	Point fast_travel_screen_searchbar_pos = { 550, 1285 };

	// Using the "S" character in beds because they do not overlap, and are not white (the default load in bg color)
	Pixel fast_travel_screen_pixel_ = Pixel({ 475, 220 }, Color(193, 245, 255));
	Pixel death_screen_pixel_ = Pixel({ 585, 220 }, Color(193, 245, 255));

	int bed_select_attempts_ = 2;
};

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

	bool IsSpawnScreenOpen(SpawnScreen screen) const;

	bool Check(int code = 0) const override;

private:
	SpawnConfig conf_;
	MouseController mouse_controller_{};
	KeyboardController keyboard_controller_{};

	bool IsDeathScreenOpen() const {
		return Screen::GetPixelColor(conf_.death_screen_pixel_.pos) == conf_.death_screen_pixel_.color;
	}

	bool IsFastTravelScreenOpen() const {
		return Screen::GetPixelColor(conf_.fast_travel_screen_pixel_.pos) == conf_.fast_travel_screen_pixel_.color;
	}

	bool IsBedSelected() const {
		return Screen::GetPixelColor(conf_.bed_of_interest_pixel.pos) == conf_.bed_of_interest_pixel.color;
	}
};

#endif