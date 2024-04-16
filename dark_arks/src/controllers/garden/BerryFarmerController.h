#ifndef DARKS_CONTROLLERS_GARDEN_BERRYFARMERCONTROLLER_H_
#define DARKS_CONTROLLERS_GARDEN_BERRYFARMERCONTROLLER_H_

#include <string>
#include <array>

#include "FloraFarmer.h"
#include "../../io/Window.h"
#include "../IDisplayCtrlPanel.h"
#include "../../io/GlobalHotKeyManager.h"
#include "../IQueueable.h"
#include "../SpawnController.h"
#include "../InventoryController.h"
#include "../MovementController.h"
#include "../CameraController.h"
#include "../SuicideController.h"
#include "../../MainThreadDispatcher.h"

namespace Darks::Controllers::Garden {
	enum class BerryType {
		Mejo = 1,
		Tinto			
	};

	class BerryFarmerConfig {
	public:
		const int CROP_HARVEST_ITERATION_COUNT = 32;
		const int FRIDGE_DEPOSIT_ITERATION_COUNT = 30;

		int autonomous_berry_farmer_interval_ = 3600000; // 1hr

		bool is_recurring_ = true;
		bool run_on_autonomous_startup_ = true;

		HotKey cancel_run_once_hotkey_ = HotKey(Key::Num9);

		/// <summary>
		/// Berries grown.
		/// </summary>
		const std::array<std::string, 4> crops_{
			"TIN",
			"TIN",
			"MEJ",
			"MEJ"
		};
	};

	class BerryFarmerController : public FloraFarmer, public IQueueable, public IDisplayCtrlPanel {
	public:
		BerryFarmerController(
			BerryFarmerConfig conf,
			GlobalHotKeyManager& hotkey_manager,
			SuicideController& suicide_controller,
			SpawnController& spawn_controller,
			InventoryController& inventory_controller,
			MovementController& movement_controller,
			CameraController& camera_controller
		);

		bool StartTimer(
			GlobalTimerManager& timer_manager,
			QueueSyncInfo& queue_info
		) override;
		bool StopTimer(GlobalTimerManager& timer_manager) override;

		/// <summary>
		/// Starts at a spawn screen and processes all loot crates.
		/// </summary>
		void Run(
			SyncInfo& info,
			GlobalTimerManager* const timer_manager = nullptr,
			QueueSyncInfo* const queue_info = nullptr,
			MainThreadDispatcher* const dispatcher = nullptr
		) override;

		void DisplayCtrlPanel() override;

		void RunOnce() {
			DARKS_INFO("BerryFarmerController was requested by the user to run once; Starting execution.");
			running_ = true;
			run_once_thread_ = std::thread([this]() {
				SyncInfo info = SyncInfo(run_once_shutdown_, run_once_cond_var_, run_once_mutex_);

				if (!suicide_controller_.Suicide(info)) {
					DARKS_INFO("Failed to suicide on manual request.");
					return;
				}

				Run(info);
			});
		}

		inline bool RunOnAutonomousStartup() const { return conf_.run_on_autonomous_startup_; }
		inline bool IsRecurring() const { return conf_.is_recurring_; }

	private:
		BerryFarmerConfig conf_;

		SuicideController& suicide_controller_;
		GlobalHotKeyManager& hotkey_manager_;

		bool running_ = false;

		// RunOnce Info
		std::thread run_once_thread_;
		std::atomic<bool> run_once_shutdown_;
		std::condition_variable run_once_cond_var_{};
		std::mutex run_once_mutex_;
	};
}
#endif