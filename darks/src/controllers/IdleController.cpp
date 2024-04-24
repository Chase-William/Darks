#include "IdleController.h"

namespace Darks::Controllers {
	void IdleController::EnterIdle(SyncInfo& info) const {
		DARKS_INFO("Entering idle state.");
		// Spawn at idle bed
		if (!spawn_controller_.Spawn(info, conf_.idle_bed_name_)) {
			auto msg = "Failed to spawn player at idle bed!";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}
		// Give plenty of time to render
		info.Wait(60000);
		// Open tribe log
		if (!tribe_log_controller_.OpenTribeLog(info)) {
			auto msg = "Failed to open tribe log to enter idle state!";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}


		// Register timer on main thread
		if (!dispatcher_.Dispatch([this]() {
			// Start polling tribe logs
			return tribe_log_controller_.StartPollingTribeLogs(timer_manager_);
			})) {
			auto msg = "Dispatch failed to register timer callback.";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}

		DARKS_INFO("Entered idle state.");
	}

	void IdleController::ExitIdle(SyncInfo& info) const {
		DARKS_INFO("Exiting idle state.");

		// Unregister timer on main thread
		if (!dispatcher_.Dispatch([this]() {
			// Stop polling tribe logs
			return tribe_log_controller_.StopPollingTribeLogs(timer_manager_);
			})) {
			auto msg = "Dispatch failed to unregister timer callback.";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}

		// Close tribe log
		if (!tribe_log_controller_.CloseTribeLog(info)) {
			auto msg = "Failed to close tribe log while in idle state!";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}
		// Look down at bed
		camera_controller_.Pivot(info, CameraDirection::Down, 3000);
		general_controller_.Use();
		info.Wait(5000); // Wait for bed fast-travel screen to popup
		// Check if successful
		if (!spawn_controller_.IsSpawnScreenOpen(SpawnScreen::FastTravelScreen)) {
			auto msg = "Failed to open bed fast-travel screen to exit idle mode!";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}
		DARKS_INFO("Exited idle state.");
	}
}