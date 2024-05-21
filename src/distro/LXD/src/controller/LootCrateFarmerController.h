#ifndef DARKS_CONTROLLERS_ISLAND_LOOTCRATEFARMERCONTROLLER_H_
#define DARKS_CONTROLLERS_ISLAND_LOOTCRATEFARMERCONTROLLER_H_

#include <string>
#include <functional>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <vector>

#include "Darks.h"

#include "LootCrates.h"

namespace LXD::Controller {
	class LootCrateFarmerConfig {
	public:
		std::string euw_bed_ = "EUW CRATE";
		bool euw_enabled_ = true;

		std::string massive_01_bed_ = "MASSIVE CRATE 1";
		bool massive_01_enabled_ = true;

		std::string massive_02_bed_ = "MASSIVE CRATE 2";
		bool massive_02_enabled_ = true;

		std::string skylord_bed_ = "SKYLORD CRATE";
		bool skylord_enabled_ = true;

		std::string swamp_red_01_bed_ = "SWAMP RED CRATE 1";
		bool swamp_red_01_enabled_ = true;
		std::string swamp_red_02_bed_ = "SWAMP RED CRATE 2";
		bool swamp_red_02_enabled_ = true;
		
		std::string swamp_yellow_01_bed_ = "SWAMP YELLOW CRATE 1";
		bool swamp_yellow_01_enabled_ = true;
		std::string swamp_yellow_02_bed_ = "SWAMP YELLOW CRATE 2";
		bool swamp_yellow_02_enabled_ = true;

		std::string swamp_blue_bed_ = "SWAMP BLUE CRATE";
		bool swamp_blue_enabled_ = true;

		std::string clever_bed_ = "CLEVER CRATE";
		bool clever_crate_enabled_ = true;

		std::string hunter_bed_ = "HUNTER CRATE";
		bool hunter_crate_enabled_ = true;

		std::string pack_bed_01_ = "PACK CRATE 1";
		bool pack_01_enabled_ = true;
		std::string pack_bed_02_ = "PACK CRATE 2";
		bool pack_02_enabled_ = true;

		bool is_recurring_ = true;
		bool run_on_autonomous_startup_ = true;

		int autonomous_farm_interval_ = 1000 * 60 * 50; // 50 minutes	

		Darks::IO::Rect screen_shot_loot_rect_ = Darks::IO::Rect(1605, 315, 2345, 560);

		Darks::IO::HotKey cancel_run_once_hotkey_ = Darks::IO::HotKey(Darks::IO::Key::Num9);
	};

	class LootCrateFarmerController : public Darks::Controller::IDisplayCtrlPanel, public Darks::Controller::IQueueable, public Darks::Controller::ISpecifyServer {
	public:
		LootCrateFarmerController(
			LootCrateFarmerConfig conf,
			Darks::IO::GlobalHotKeyManager& hotkey_manager,
			Darks::Controller::InventoryController& inventory_controller,
			Darks::Controller::CameraController& camera_controller,
			Darks::Controller::GeneralController& general_controller,
			Darks::Controller::SpawnController& spawn_controller,
			Darks::Controller::SuicideController& suicide_controller,
			Darks::Controller::MovementController& movement_controller
		);

		void DisplayCtrlPanel() override;
		std::string GetServer() const override { return "2134"; };

		bool StartTimer(
			Darks::GlobalTimerManager& timer_manager,
			Darks::Controller::QueueSyncInfo& queue_info
		) override;
		bool StopTimer(Darks::GlobalTimerManager& timer_manager) override;

		inline void RunOnce() {
			DARKS_INFO("LootCrateFarmController was requested by the user to run once; Starting execution.");
			running_ = true; // Update running state here before creating new thread as it could take awhile
			std::thread([this]() {
				Darks::SyncInfo info = Darks::SyncInfo(run_once_shutdown_, run_once_cond_var_, run_once_mutex_);

				if (!suicide_controller_.Suicide(info)) {
					DARKS_INFO("Failed to suicide on manual request.");
					return;
				}

				Run(info);
			})
			.detach();
		}

		/// <summary>
		/// Starts at a spawn screen and processes all loot crates.
		/// </summary>
		void Run(
			Darks::SyncInfo& info,
			Darks::GlobalTimerManager* const timer_manager = nullptr,
			Darks::Controller::QueueSyncInfo* const queue_info = nullptr,
			Darks::MainThreadDispatcher* const dispatcher = nullptr
		) override;

		inline bool RunOnStartup() const override { return conf_.run_on_autonomous_startup_;  }
		inline bool IsRecurring() const override { return conf_.is_recurring_; }

		/// <summary>
		/// Fires whenever a new loot crate has been opened automatically.
		/// </summary>
		std::function<void(const std::string& name, std::vector<char>& jpg_buf)> on_crate_;

	private:
		int timer_id_ = 0;

		std::unique_ptr<std::vector<LootCrateInfo*>> crates_;
		LootCrateFarmerConfig conf_;
		Darks::Controller::SpawnController& spawn_controller_;

		Darks::Controller::SuicideController& suicide_controller_;
		Darks::IO::GlobalHotKeyManager& hotkey_manager_;
				
		bool running_ = false;

		// RunOnce info
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