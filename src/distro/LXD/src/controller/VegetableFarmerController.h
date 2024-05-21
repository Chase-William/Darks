#ifndef DARKS_CONTROLLERS_GARDEN_VEGETABLECONTROLLER_H_
#define DARKS_CONTROLLERS_GARDEN_VEGETABLECONTROLLER_H_

#include <string>
#include <array>
#include <condition_variable>
#include <optional>
#include <atomic>
#include <mutex>

#include "Darks.h"

#include "FloraFarmer.h"

namespace LXD::Controller {
	enum class CropType {
		Citronal = 3,
		Longrass,
		Savoroot,
		Rockarrot
	};

	class VegetableFarmerConf {
	public:
		const int CROP_HARVEST_ITERATION_COUNT = 70;
		const int FRIDGE_DEPOSIT_ITERATION_COUNT = 12;

		int autonomous_vegetable_farmer_interval_ = 10800000; // 3hrs;

		bool is_recurring_ = false;
		bool run_on_autonomous_startup_ = false;

		Darks::IO::HotKey cancel_run_once_hotkey_ = Darks::IO::HotKey(Darks::IO::Key::Num9);

		/// <summary>
		/// Special grown crops.
		/// </summary>
		const std::array<std::string, 4> crops_{
			"CITR",
			"LONG",
			"SAVO",
			"ROCK"
		};
	};

	class VegetableFarmerController : public FloraFarmer, public Darks::Controller::IQueueable, public Darks::Controller::IDisplayCtrlPanel, public Darks::Controller::ISpecifyServer{
	public:
		VegetableFarmerController(
			VegetableFarmerConf conf,
			Darks::IO::GlobalHotKeyManager& hotkey_manager,
			Darks::Controller::SuicideController& suicide_controller,
			Darks::Controller::SpawnController& spawn_controller,
			Darks::Controller::InventoryController& inventory_controller,
			Darks::Controller::MovementController& movement_controller,
			Darks::Controller::CameraController& camera_controller
		);

		std::string GetServer() const override { return "2134"; };
		void DisplayCtrlPanel() override;

		bool StartTimer(
			Darks::GlobalTimerManager& timer_manager,
			Darks::Controller::QueueSyncInfo& queue_info
		) override;
		bool StopTimer(Darks::GlobalTimerManager& timer_manager) override;

		void RunOnce() {
			DARKS_INFO("VegetableFarmerController was requested by the user to run once; Starting execution.");
			running_ = true;
			std::thread([this]() {
				Darks::SyncInfo info = Darks::SyncInfo(run_once_shutdown_, run_once_cond_var_, run_once_mutex_);
				
				if (!suicide_controller_.Suicide(info)) {
					DARKS_INFO("Failed to suicide on manual request.");
					return;
				}

				Run(info);
			}).detach();
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

		inline bool RunOnStartup() const override { return conf_.run_on_autonomous_startup_; }
		inline bool IsRecurring() const override { return conf_.is_recurring_; }

	private:
		VegetableFarmerConf conf_;

		Darks::Controller::SuicideController& suicide_controller_;
		Darks::IO::GlobalHotKeyManager& hotkey_manager_;

		bool running_ = false;

		// RunOnce Info
		std::atomic<bool> run_once_shutdown_;
		std::condition_variable run_once_cond_var_{};
		std::mutex run_once_mutex_;
	};
}

#endif