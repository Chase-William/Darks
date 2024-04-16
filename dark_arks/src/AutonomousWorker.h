#ifndef DARKS_AUTONOMOUSWORKER_H_
#define DARKS_AUTONOMOUSWORKER_H_

#include <thread>
#include <condition_variable>
#include <exception>
#include <cassert>
#include <queue>

#include "io/Window.h"
#include "GlobalTimerManager.h"
#include "controllers/IQueueable.h"
#include "controllers/IDisplayCtrlPanel.h"
#include "controllers/InventoryController.h"
#include "controllers/SuicideController.h"
#include "controllers/SpawnController.h"
#include "controllers/island/LootCrateFarmerController.h"
#include "controllers/garden/VegetableFarmerController.h"
#include "controllers/garden/BerryFarmerController.h"
#include "controllers/IdleController.h"
#include "controllers/ProcessController.h"
#include "MainThreadDispatcher.h"
#include "SyncInfo.h"

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
		InventoryController& inventory_controller,
		SuicideController& suicide_controller,
		SpawnController& spawn_controller,
		Darks::Controllers::Island::LootCrateFarmerController& loot_crate_farmer_controller,
		Darks::Controllers::Garden::VegetableFarmerController& vegetable_farmer_controller,
		Darks::Controllers::Garden::BerryFarmerController& berry_farmer_controller,
		IdleController& idle_controller,
		ProcessController& process_controller,
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

	QueueSyncInfo& GetQueueSyncInfo() const { return *queue_info_; }
	MainThreadDispatcher& GetDispatcher() const { return main_thread_dispatcher_; }

private:
	GlobalTimerManager& timer_manager_;
	InventoryController& inventory_controller_;
	SuicideController& suicide_controller_;
	SpawnController& spawn_controller_;
	Darks::Controllers::Island::LootCrateFarmerController& loot_crate_farmer_controller_;
	Darks::Controllers::Garden::VegetableFarmerController& vegetable_farmer_controller_;
	Darks::Controllers::Garden::BerryFarmerController& berry_farmer_controller_;
	IdleController& idle_controller_;
	ProcessController& process_controller_;
	
	MainThreadDispatcher& main_thread_dispatcher_;

	std::thread thread_;
	std::atomic<bool> shutdown_{ true };

	std::queue<IQueueable*> work_queue_;
	std::mutex work_queue_mutex_{};

	// Main loop synchronization objects
	std::mutex main_queue_loop_mutex_{};
	std::condition_variable main_queue_loop_var_{}; // Notify when manually shutting down or new job added to queue
	// Controller synchronization objects
	std::mutex controller_mutex_{};
	std::condition_variable controller_var_{}; // Notify when manually shutting down

	/*SyncInfo controller_info = SyncInfo(shutdown_, controller_var_, controller_mutex_);
	QueueSyncInfo queue_info = QueueSyncInfo(&work_queue_, work_queue_mutex_, main_queue_loop_var_);*/
	std::unique_ptr<QueueSyncInfo> queue_info_;
};

#endif