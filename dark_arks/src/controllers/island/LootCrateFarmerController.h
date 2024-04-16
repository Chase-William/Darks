#ifndef DARKS_CONTROLLERS_ISLAND_LOOTCRATEFARMERCONTROLLER_H_
#define DARKS_CONTROLLERS_ISLAND_LOOTCRATEFARMERCONTROLLER_H_

#include <string>
#include <stdexcept>
#include <queue>
#include <functional>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <vector>

#include "imgui/imgui.h"

#include "../../Log.h"
#include "../../SyncInfo.h"
#include "../DisplayController.h"
#include "../SpawnController.h"
#include "../SuicideController.h"
#include "../../io/GlobalHotKeyManager.h"
#include "LootCrates.h"
#include "../../GlobalTimerManager.h"
#include "../IQueueable.h"
#include "../../io/Window.h"
#include "../../MainThreadDispatcher.h"

namespace Darks::Controllers::Island {
	class LootCrateFarmerConfig {
	public:
		std::string euw_bed_ = "EUW CRATE";
		bool euw_enabled_ = true;

		std::string massive_bed_ = "MASSIVE CRATE";
		bool massive_enabled_ = true;

		std::string skylord_bed_ = "SKYLORD CRATE";
		bool skylord_enabled_ = true;

		std::string swamp_bed_01_ = "SWAMP CRATE 1";
		bool swamp_01_enabled_ = true;
		std::string swamp_bed_02_ = "SWAMP CRATE 2";
		bool swamp_02_enabled_ = true;
		
		std::string pack_bed_01_ = "PACK CRATE 1";
		bool pack_01_enabled_ = true;
		std::string pack_bed_02_ = "PACK CRATE 2";
		bool pack_02_enabled_ = true;

		bool is_recurring_ = true;
		bool run_on_autonomous_startup_ = true;

		int autonomous_farm_interval_ = 1000 * 60 * 50; // 50 minutes	

		Rect screen_shot_loot_rect_ = Rect(1605, 315, 2345, 560);

		HotKey cancel_run_once_hotkey_ = HotKey(Key::Num9);
	};

	class LootCrateFarmerController : public IDisplayCtrlPanel, public IQueueable {
	public:
		LootCrateFarmerController(
			LootCrateFarmerConfig conf,
			GlobalHotKeyManager& hotkey_manager,
			InventoryController& inventory_controller,
			CameraController& camera_controller,
			GeneralController& general_controller,
			SpawnController& spawn_controller,
			SuicideController& suicide_controller,
			MovementController& movement_controller
		);

		void DisplayCtrlPanel() override;

		bool StartTimer(
			GlobalTimerManager& timer_manager,
			QueueSyncInfo& queue_info
		) override;
		bool StopTimer(GlobalTimerManager& timer_manager) override;

		inline void RunOnce() {
			DARKS_INFO("LootCrateFarmController was requested by the user to run once; Starting execution.");
			running_ = true; // Update running state here before creating new thread as it could take awhile
			run_once_thread_ = std::thread([this]() {
				SyncInfo info = SyncInfo(run_once_shutdown_, run_once_cond_var_, run_once_mutex_);

				if (!suicide_controller_.Suicide(info)) {
					DARKS_INFO("Failed to suicide on manual request.");
					return;
				}

				Run(info);
			});
		}

		/// <summary>
		/// Starts at a spawn screen and processes all loot crates.
		/// </summary>
		void Run(
			SyncInfo& info,			
			GlobalTimerManager* const timer_manager = nullptr,
			QueueSyncInfo* const queue_info = nullptr,
			MainThreadDispatcher* const dispatcher = nullptr
		) override;

		inline bool RunOnAutonomousStartup() const { return conf_.run_on_autonomous_startup_;  }
		inline bool IsRecurring() const { return conf_.is_recurring_; }

		/// <summary>
		/// Fires whenever a new loot crate has been opened automatically.
		/// </summary>
		std::function<void(const std::string& name, std::vector<char>& jpg_buf)> on_crate_;



	private:
		int timer_id_ = 0;

		std::unique_ptr<std::vector<LootCrateInfo*>> crates_;
		LootCrateFarmerConfig conf_;
		SpawnController& spawn_controller_;

		SuicideController& suicide_controller_;
		GlobalHotKeyManager& hotkey_manager_;
				
		bool running_ = false;

		// RunOnce info
		std::thread run_once_thread_;
		std::atomic<bool> run_once_shutdown_;
		std::condition_variable run_once_cond_var_{};
		std::mutex run_once_mutex_;

		void OnCrateOpened(const std::string& crate_name, std::vector<char>& jpg_buf) const {
			if (on_crate_)
				on_crate_(crate_name, jpg_buf);
		}
	};
}

#endif