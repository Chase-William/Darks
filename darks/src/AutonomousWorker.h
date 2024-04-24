#ifndef DARKS_AUTONOMOUSWORKER_H_
#define DARKS_AUTONOMOUSWORKER_H_

#include <thread>
#include <condition_variable>
#include <exception>
#include <cassert>
#include <queue>

#include "Log.h"
#include "io/Window.h"
#include "GlobalTimerManager.h"
#include "controllers/IQueueable.h"
#include "controllers/ISpecifyServer.h"
#include "controllers/IDisplayCtrlPanel.h"
#include "controllers/InventoryController.h"
#include "controllers/SuicideController.h"
#include "controllers/SpawnController.h"
#include "controllers/servers/2134/LootCrateFarmerController.h"
#include "controllers/servers/2134/VegetableFarmerController.h"
#include "controllers/servers/2134/BerryFarmerController.h"
#include "controllers/IdleController.h"
#include "controllers/ProcessController.h"
#include "controllers/ServerController.h"
#include "MainThreadDispatcher.h"
#include "SyncInfo.h"

namespace Darks {
	class QueueInfo {
	public:
		QueueInfo(std::queue<std::function<void()>>& work_queue, std::mutex work_queue_mutex
		) :
			work_queue_(work_queue),
			work_queue_mutex_(work_queue_mutex)
		{ }

		std::queue<std::function<void()>>& work_queue_;
		std::mutex& work_queue_mutex_;
	};

	// Probably take a controller as the default as an option in the future

	class AutonomousWorker {
	public:
		AutonomousWorker(
			GlobalTimerManager& timer_manager,
			Darks::Controllers::InventoryController& inventory_controller,
			Darks::Controllers::SuicideController& suicide_controller,
			Darks::Controllers::SpawnController& spawn_controller,
			Darks::Controllers::Servers::_2134::LootCrateFarmerController& loot_crate_farmer_controller,
			Darks::Controllers::Servers::_2134::VegetableFarmerController& vegetable_farmer_controller,
			Darks::Controllers::Servers::_2134::BerryFarmerController& berry_farmer_controller,
			Darks::Controllers::IdleController& idle_controller,
			Darks::Controllers::ProcessController& process_controller,
			Darks::Controllers::ServerController& server_controller,
			MainThreadDispatcher& main_thread_dispatcher
		) :
			timer_manager_(timer_manager),
			inventory_controller_(inventory_controller),
			suicide_controller_(suicide_controller),
			spawn_controller_(spawn_controller),
			loot_crate_farmer_controller_(loot_crate_farmer_controller),
			vegetable_farmer_controller_(vegetable_farmer_controller),
			berry_farmer_controller_(berry_farmer_controller),
			idle_controller_(idle_controller),
			process_controller_(process_controller),
			server_controller_(server_controller),
			main_thread_dispatcher_(main_thread_dispatcher)
		{ }

		/// <summary>
		/// Starts the autonomous worker.
		/// </summary>
		/// <returns>True if successfully started, false otherwise.</returns>
		void Start();
		void Stop();

		/// <summary>
		/// Displays the control panel for the worker using imgui.
		/// </summary>
		/// <returns>True if the autonomous worker has requested UI components become hidden, false otherwise.</returns>
		bool DisplayCtrlPanel();

		bool IsRunning() { return !shutdown_; }

		Darks::Controllers::QueueSyncInfo& GetQueueSyncInfo() const { return *queue_info_; }
		MainThreadDispatcher& GetDispatcher() const { return main_thread_dispatcher_; }

	private:
		std::string current_server_ = "2134"; // Currently we assume we are always starting on home server 2134

		GlobalTimerManager& timer_manager_;
		Darks::Controllers::InventoryController& inventory_controller_;
		Darks::Controllers::SuicideController& suicide_controller_;
		Darks::Controllers::SpawnController& spawn_controller_;
		Darks::Controllers::Servers::_2134::LootCrateFarmerController& loot_crate_farmer_controller_;
		Darks::Controllers::Servers::_2134::VegetableFarmerController& vegetable_farmer_controller_;
		Darks::Controllers::Servers::_2134::BerryFarmerController& berry_farmer_controller_;
		Darks::Controllers::IdleController& idle_controller_;
		Darks::Controllers::ProcessController& process_controller_;
		Darks::Controllers::ServerController& server_controller_;

		MainThreadDispatcher& main_thread_dispatcher_;

		std::thread thread_;
		std::atomic<bool> shutdown_{ true };

		std::queue<Darks::Controllers::IQueueable*> work_queue_;
		std::mutex work_queue_mutex_{};

		// Main loop synchronization objects
		std::mutex main_queue_loop_mutex_{};
		std::condition_variable main_queue_loop_var_{}; // Notify when manually shutting down or new job added to queue
		// Controller synchronization objects
		std::mutex controller_mutex_{};
		std::condition_variable controller_var_{}; // Notify when manually shutting down

		/*SyncInfo controller_info = SyncInfo(shutdown_, controller_var_, controller_mutex_);
		QueueSyncInfo queue_info = QueueSyncInfo(&work_queue_, work_queue_mutex_, main_queue_loop_var_);*/
		std::unique_ptr<Darks::Controllers::QueueSyncInfo> queue_info_;
	};
}

#endif