#pragma once

#include <thread>
#include <condition_variable>
#include <exception>
#include <cassert>

#include "controllers/InventoryController.h"
#include "controllers/SuicideController.h"
#include "TaskInfo.h"

// Probably take a controller as the default as an option in the future

class AutonomousWorker {
public:
	AutonomousWorker(
		InventoryController& inventory_controller,
		SuicideController& suicide_controller
	) :
		inventory_controller_(inventory_controller),
		suicide_controller_(suicide_controller)
	{ }

	void Start();
	void Stop();

	bool IsRunning() { return !shutdown_; }
private:
	InventoryController& inventory_controller_;
	SuicideController& suicide_controller_;

	std::atomic<bool> shutdown_{true};
	std::mutex mutex_;
	std::condition_variable cond_var_;
	std::thread thread_;
};

