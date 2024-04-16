#ifndef DARKS_CONTROLLERS_IQUEUEABLE_H_
#define DARKS_CONTROLLERS_IQUEUEABLE_H_

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

#include "../SyncInfo.h"
#include "../GlobalTimerManager.h"
#include "../MainThreadDispatcher.h"

class IQueueable; // Forward declare for compilation

class QueueSyncInfo {
public:
	QueueSyncInfo(
		std::queue<IQueueable*>* work_queue,
		std::mutex& work_queue_mutex,
		std::condition_variable& queue_var
	);

	std::queue<IQueueable*>* work_queue_;
	std::mutex& work_queue_mutex_;
	std::condition_variable& queue_var_;
};

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
};

#endif