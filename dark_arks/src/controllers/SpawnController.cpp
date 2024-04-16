#include "SpawnController.h"

SpawnController::SpawnController(SpawnConfig conf) :
	conf_(conf)
{ }

bool SpawnController::Spawn(
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
) const {
	// If neither spawn screen or fast travel screen are open after waiting, return false
	if (!info.Wait(*this, wait_for_spawn_screen_poll_interval, wait_for_spawn_screen_timeout, SpawnScreen::DeathScreen | SpawnScreen::FastTravelScreen)) {
		DARKS_WARN("Failed to receive spawn or fast travel screens.");
		return false;
	}

	info.Wait(after_spawn_screen_visible);

	if (IsFastTravelScreenOpen()) { // Click on fast travel searchbar
		mouse_controller_.Click(conf_.fast_travel_screen_searchbar_pos);
	}
	else if (IsDeathScreenOpen()) { // Click on death screen searchbar
		mouse_controller_.Click(conf_.death_screen_searchbar_pos);
	}
	else {
		DARKS_WARN("Spawn screen wasn't open.");
		return false;
	}

	info.Wait(after_searchbar_focus_click);

	// Enter query string
	keyboard_controller_.Keystrokes(bed_name);
	info.Wait(after_query_str_entered);

	// Select bed, with multiple attempts as sometimes a click doesn't register on the bed for some unknown reason
	// Possibily the move to and click at destination can occur to fast for target, greatly reduces chance
	for (int i = 0; i < conf_.bed_select_attempts_; i++) {
		if (i == 0) { // First click moves mouse and clicks, move always works, however selection can fail at new location
			mouse_controller_.Click(conf_.bed_of_interest_pixel.pos);
		}
		else { // Incase bed selection failed for an unknown, probably lag related issue, just perform click again at already set location
			mouse_controller_.Click();
		}
		info.Wait(after_bed_selection_click);
		// If the bed was successfully selected on the first try, break out
		if (IsBedSelected())
			break;
	}
	
	// Determine if a bed was successfully selected after search and click
	if (!IsBedSelected()) {
		DARKS_WARN(std::format("Failed to select bed: {}.", bed_name));
		return false;
	}

	// Click spawn button
	mouse_controller_.Click(conf_.spawn_btn_pos);
	// Check if all varients of spawning screens have closed indicating the player is beginning to spawn
	auto status = info.Wait(*this, wait_for_disappearance_of_spawn_screen_poll_interval, wait_for_disappearance_of_spawn_screen_timeout);
	DARKS_INFO(std::format("{} in.", status ? "Successfully spawned" : "Failed to spawn"));
	return status;
}

bool SpawnController::IsSpawnScreenOpen(SpawnScreen screen) const {
	// Bitwise checks to check screen based on
	if ((screen & SpawnScreen::FastTravelScreen) && (screen & SpawnScreen::DeathScreen)) {
		return this->IsDeathScreenOpen() || this->IsFastTravelScreenOpen();
	}
	else if (screen & SpawnScreen::FastTravelScreen) {
		return this->IsFastTravelScreenOpen();
	}
	else if (screen & SpawnScreen::DeathScreen) {
		return this->IsDeathScreenOpen();
	}
	else {
		throw std::invalid_argument(std::format("Invalid argument {} given for screen param.", static_cast<int>(screen)));
	}
}

bool SpawnController::Check(int code) const {
	switch (code) {
	// Default check for testing if character has spawned yet via the disappearance of the fast travel or spawn screen 
	case 0:
		// Check that neither screen is open after spawning should have started
		return !IsFastTravelScreenOpen() && !IsDeathScreenOpen();
	// Check if death screen or fast travel screen is open
	case static_cast<int>(SpawnScreen::DeathScreen) + static_cast<int>(SpawnScreen::FastTravelScreen):
		return IsSpawnScreenOpen(static_cast<SpawnScreen>(code));	
	default:
		auto msg = std::format("Invalid code of {} given to SpawnController check method", code);
		DARKS_ERROR(msg);
		throw std::invalid_argument(msg);
	}	
}
