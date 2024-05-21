#ifndef DARKS_CONTROLLERS_IQUEUEABLE_H_
#define DARKS_CONTROLLERS_IQUEUEABLE_H_

#include <queue>
#include <mutex>
#include <condition_variable>

#include "../SyncInfo.h"
#include "../GlobalTimerManager.h"
#include "../MainThreadDispatcher.h"

namespace Darks::Controller {
	class IQueueable; // Forward declare for compilation

	class QueueSyncInfo {
	public:
		QueueSyncInfo(
			std::queue<IQueueable*>* work_queue,
			int& error_code,
			std::mutex& work_queue_mutex,
			std::condition_variable& queue_var
		);

		std::queue<IQueueable*>* work_queue_;
		/// <summary>
		/// error_code_ can be set without thread protection as two queue jobs cannot happen at once and even if that did happen,
		/// all errors after possible custom recovery logic resort to the same fallback procedure which always works.
		/// </summary>
		int& error_code_;
		std::mutex& work_queue_mutex_;
		std::condition_variable& queue_var_;
	};

	/// <summary>
	/// Implement to take advantage of the queueing system.
	/// </summary>
	class IQueueable {
	public:
		/// <summary>
		/// Starts a timer that will post work to a queue when elapsed.
		/// </summary>
		/// <param name="timer_manager"></param>
		/// <param name="queue_info">A copy of a queue info object containing references to synchronizationa objects for the queue used by the timer's posted callback.</param>
		/// <returns></returns>
		virtual bool StartTimer(
			GlobalTimerManager& timer_manager,
			QueueSyncInfo& queue_info
		) = 0;
		virtual bool StopTimer(GlobalTimerManager& timer_manager) = 0;

		/// <summary>
		/// Starts at a spawn screen and processes all loot crates.
		/// </summary>
		virtual void Run(
			SyncInfo& info,
			GlobalTimerManager* const timer_manager = nullptr,
			QueueSyncInfo* const queue_info = nullptr,
			MainThreadDispatcher* const main_thread_dispatcher = nullptr
		) = 0;

		virtual bool RunOnStartup() const = 0;
		virtual bool IsRecurring() const = 0;
		virtual bool IsRunning() const = 0;
	};
}

#endif