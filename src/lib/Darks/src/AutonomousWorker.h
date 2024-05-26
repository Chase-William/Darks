#ifndef DARKS_AUTONOMOUSWORKER_H_
#define DARKS_AUTONOMOUSWORKER_H_

#include <thread>
#include <condition_variable>
#include <exception>
#include <cassert>
#include <queue>
#include <vector>

#include "Log.h"
#include "io/Window.h"
#include "GlobalTimerManager.h"
#include "controller/IQueueable.h"
#include "controller/ISpecifyServer.h"
#include "controller/IDisplayCtrlPanel.h"
#include "controller/InventoryController.h"
#include "controller/SuicideController.h"
#include "controller/SpawnController.h"
#include "controller/IdleController.h"
#include "controller/ProcessController.h"
#include "controller/ServerController.h"
#include "controller/TribeLogController.h"
#include "controller/ParasaurAlarmController.h"
#include "MainThreadDispatcher.h"
#include "SyncInfo.h"

namespace Darks {
	/*class QueueInfo {
	public:
		QueueInfo(std::queue<std::function<void()>>& work_queue, std::mutex work_queue_mutex
		) :
			work_queue_(work_queue),
			work_queue_mutex_(work_queue_mutex)
		{ }

		std::queue<std::function<void()>>& work_queue_;
		std::mutex& work_queue_mutex_;
	};*/

	// Probably take a controller as the default as an option in the future

	class AutonomousWorker {
	public:
		AutonomousWorker(
			GlobalTimerManager& timer_manager,			
			MainThreadDispatcher& main_thread_dispatcher,
			Controller::SuicideController& suicide_controller,
			Controller::SpawnController& spawn_controller,
			Controller::IdleController& idle_controller,
			Controller::ProcessController& process_controller,
			Controller::ServerController& server_controller,
			Controller::TribeLogController& tribe_log_controller,
			Controller::ParasaurAlarmController& parasaur_alarm_controller,
			std::unique_ptr<std::vector<Controller::IQueueable*>> queueables
		) :
			timer_manager_(timer_manager),
			suicide_controller_(suicide_controller),
			spawn_controller_(spawn_controller),
			idle_controller_(idle_controller),
			process_controller_(process_controller),
			server_controller_(server_controller),
			tribe_log_controller_(tribe_log_controller),
			parasaur_alarm_controller_(parasaur_alarm_controller),
			main_thread_dispatcher_(main_thread_dispatcher)
		{ 
			queueables_ = std::move(queueables); // Take ownership of queueables via move
		}

		/// <summary>
		/// Starts the autonomous worker.
		/// </summary>
		/// <returns>True if successfully started, false otherwise.</returns>
		void Start();
		void Stop();

		std::function<void()> on_started_;
		std::function<void()> on_stopped_;

		// std::function<void(bool is_running)> on_controller_run_once_state_changed_;

		/// <summary>
		/// Displays the control panel for the worker using imgui.
		/// </summary>
		void DisplayCtrlPanel();

		bool IsRunning() { return !shutdown_; }

		Darks::Controller::QueueSyncInfo& GetQueueSyncInfo() const { return *queue_info_; }
		MainThreadDispatcher& GetDispatcher() const { return main_thread_dispatcher_; }

	private:
		std::unique_ptr<std::vector<Controller::IQueueable*>> queueables_;

		// std::string current_server_ = "2134"; // Currently we assume we are always starting on home server 2134

		GlobalTimerManager& timer_manager_;
		MainThreadDispatcher& main_thread_dispatcher_;

		/*
			Required controllers for autonomous mode to function
		*/
		Controller::SuicideController& suicide_controller_;
		Controller::SpawnController& spawn_controller_;
		Controller::IdleController& idle_controller_;
		Controller::ProcessController& process_controller_;
		Controller::ServerController& server_controller_;	
		Controller::TribeLogController& tribe_log_controller_;
		Controller::ParasaurAlarmController& parasaur_alarm_controller_;
		Controller::MouseController mouse_controller_{};

		inline void SetIsRunning(bool value) {			
			if (shutdown_ == !value)
				return;
			shutdown_ = !value;
			if (shutdown_)
				on_stopped_();
			else
				on_started_();			
		}
		std::atomic<bool> shutdown_{ true };

		// Queue related synchronization objects
		std::queue<Darks::Controller::IQueueable*> work_queue_;
		std::mutex work_queue_mutex_{};
		int error_code_ = 0;

		// Main loop synchronization objects
		std::mutex main_queue_loop_mutex_{};
		std::condition_variable main_queue_loop_var_{}; // Notify when manually shutting down or new job added to queue
		// Controller synchronization objects
		std::mutex controller_mutex_{};
		std::condition_variable controller_var_{}; // Notify when manually shutting down

		/*SyncInfo controller_info = SyncInfo(shutdown_, controller_var_, controller_mutex_);
		QueueSyncInfo queue_info = QueueSyncInfo(&work_queue_, work_queue_mutex_, main_queue_loop_var_);*/
		std::unique_ptr<Darks::Controller::QueueSyncInfo> queue_info_;

		void Cleanup();
	};
}

#endif