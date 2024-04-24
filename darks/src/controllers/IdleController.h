#ifndef DARKS_CONTROLLERS_IDLECONTROLLER_H_
#define DARKS_CONTROLLERS_IDLECONTROLLER_H_

#include "../Log.h"
#include "../SyncInfo.h"
#include "SpawnController.h"
#include "TribeLogController.h"
#include "CameraController.h"
#include "GeneralController.h"
#include "../GlobalTimerManager.h"
#include "../MainThreadDispatcher.h"

namespace Darks::Controllers {
	class IdleConfig {
	public:
		std::string idle_bed_name_ = "LILXD CARNO RESTING";
	};

	/// <summary>
	/// Used for entering the player into an idle state.
	/// </summary>
	class IdleController {
	public:
		IdleController(
			IdleConfig conf,
			MainThreadDispatcher& dispatcher,
			GlobalTimerManager& timer_manager,
			SpawnController& spawn_controller,
			TribeLogController& tribe_log_controller,
			CameraController& camera_controller,
			GeneralController& general_controller
		) :
			conf_(conf),
			dispatcher_(dispatcher),
			timer_manager_(timer_manager),
			spawn_controller_(spawn_controller),
			tribe_log_controller_(tribe_log_controller),
			camera_controller_(camera_controller),
			general_controller_(general_controller)
		{ }

		/// <summary>
		/// Enters the player into idle state.
		/// </summary>
		/// <param name="info"></param>
		void EnterIdle(SyncInfo& info) const;
		/// <summary>
		/// Exits the player from idle state.
		/// </summary>
		/// <param name="info"></param>
		void ExitIdle(SyncInfo& info) const;

		bool IsIdle() const { return is_idle_; }

	private:
		IdleConfig conf_;
		MainThreadDispatcher& dispatcher_;
		GlobalTimerManager& timer_manager_;
		SpawnController& spawn_controller_;
		TribeLogController& tribe_log_controller_;
		CameraController& camera_controller_;
		GeneralController& general_controller_;

		bool is_idle_ = false;
	};
}

#endif