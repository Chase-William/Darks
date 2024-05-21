#ifndef DARKS_CONTROLLERS_GARDEN_BERRYFARMERCONTROLLER_H_
#define DARKS_CONTROLLERS_GARDEN_BERRYFARMERCONTROLLER_H_

#include <string>
#include <array>

#include "Darks.h"

#include "FloraFarmer.h"

namespace LXD::Controller {
	enum class BerryType {
		Mejo = 1,
		Tinto			
	};

	class BerryFarmerConfig {
	public:
		const int CROP_HARVEST_ITERATION_COUNT = 26;
		const int FRIDGE_DEPOSIT_ITERATION_COUNT = 30;

		int autonomous_berry_farmer_interval_ = 3600000; // 1hr

		bool is_recurring_ = true;
		bool run_on_autonomous_startup_ = true;

		Darks::IO::HotKey cancel_run_once_hotkey_ = Darks::IO::HotKey(Darks::IO::Key::Num9);

		/// <summary>
		/// Berries grown.
		/// </summary>
		const std::array<std::string, 3> crops_{
			"MEJ",
			"MEJ",
			"MEJ",
		};
	};

	class BerryFarmerController : public FloraFarmer, public Darks::Controller::IQueueable, public Darks::Controller::IDisplayCtrlPanel, public Darks::Controller::ISpecifyServer {
	public:
		BerryFarmerController(
			BerryFarmerConfig conf,
			Darks::IO::GlobalHotKeyManager& hotkey_manager,
			Darks::Controller::SuicideController& suicide_controller,
			Darks::Controller::SpawnController& spawn_controller,
			Darks::Controller::InventoryController& inventory_controller,
			Darks::Controller::MovementController& movement_controller,
			Darks::Controller::CameraController& camera_controller
		);

		void DisplayCtrlPanel() override;
		std::string GetServer() const override { return "2134"; };

		bool StartTimer(
			Darks::GlobalTimerManager& timer_manager,
			Darks::Controller::QueueSyncInfo & queue_info
		) override;
		bool StopTimer(Darks::GlobalTimerManager& timer_manager) override;

		/// <summary>
		/// Starts at a spawn screen and processes all loot crates.
		/// </summary>
		void Run(
			Darks::SyncInfo& info,
			Darks::GlobalTimerManager* const timer_manager = nullptr,
			Darks::Controller::QueueSyncInfo* const queue_info = nullptr,
			Darks::MainThreadDispatcher* const dispatcher = nullptr
		) override;		

		void RunOnce() {
			DARKS_INFO("BerryFarmerController was requested by the user to run once; Starting execution.");
			running_ = true;
			run_once_thread_ = std::thread([this]() {
				Darks::SyncInfo info = Darks::SyncInfo(run_once_shutdown_, run_once_cond_var_, run_once_mutex_);

				if (!suicide_controller_.Suicide(info)) {
					DARKS_INFO("Failed to suicide on manual request.");
					return;
				}

				Run(info);
			});
		}

		inline bool RunOnStartup() const override { return conf_.run_on_autonomous_startup_; }
		inline bool IsRecurring() const override { return conf_.is_recurring_; }

	private:
		BerryFarmerConfig conf_;

		Darks::Controller::SuicideController& suicide_controller_;
		Darks::IO::GlobalHotKeyManager& hotkey_manager_;

		bool running_ = false;

		// RunOnce Info
		std::thread run_once_thread_;
		std::atomic<bool> run_once_shutdown_;
		std::condition_variable run_once_cond_var_{};
		std::mutex run_once_mutex_;
	};
}
#endif