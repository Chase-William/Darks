#ifndef DARKS_CONTROLLERS_SUICIDECONTROLLER_H_
#define DARKS_CONTROLLERS_SUICIDECONTROLLER_H_

#include <condition_variable>
#include <expected>

#include "../Log.h"
#include "../io/Pixel.h"
#include "../SyncInfo.h"
#include "InventoryController.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "SpawnController.h"

namespace Darks::Controllers {
	class SuicideConfig {
	public:
		Darks::IO::Point implant_pos_ = { 300, 370 };
		int implant_ready_delay_ = 7000;
		// Pixel suicide_cooldown_pixel = Pixel({ 580, 790 }, Color(255, 89, 89));
	};

	class SuicideController : public ICheckable {
	public:
		SuicideController(
			SuicideConfig conf,
			SpawnController& spawn_controller,
			InventoryController& inventory_controller);

		/// <summary>
		/// Makes the player commit suicide, returning them to the spawn screen. Will open the player inventory if needed.
		/// </summary>
		/// <param name="info">Sync info used for sleeping the thread.</param>
		/// <param name="wait_for_inventory_open_poll_interval">The poll interval used when checking inventory open status.</param>
		/// <param name="wait_for_inventory_open_timeout">The max amount of time permitted to pass when checking inventory open status.</param>
		/// <param name="wait_for_disappearing_spawn_screen_poll_interval">The poll interval used when checking for the disappearance of the spawn screen.</param>
		/// <param name="wait_for_disappearing_spawn_screen_timeout">The max amount of time permitted to pass when checking for the disappearance of the spawn screen.</param>
		/// <returns></returns>
		bool Suicide(
			SyncInfo& info,
			int wait_for_inventory_open_poll_interval,
			int wait_for_inventory_open_timeout,
			int wait_for_disappearing_spawn_screen_poll_interval,
			int wait_for_disappearing_spawn_screen_timeout
		) const;
		bool Suicide(SyncInfo& info) const {
			return Suicide(info, 250, 10000, 250, 10000);
		}

		bool Check(int code = 0) const override;
	private:
		SuicideConfig conf_;
		InventoryController& inventory_controller_;
		SpawnController& spawn_controller_;
		KeyboardController keyboard_controller_{};
		MouseController mouse_controller_{};
	};
}

#endif