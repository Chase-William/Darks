#include "AutonomousWorker.h"

void AutonomousWorker::Start() {
	// Prevent extra calls
	if (!shutdown_)
		return;

	shutdown_ = false;
	thread_ = std::thread([this]() {
		/*HWND hwnd = FindWindow(nullptr, L"ArkAscended");
		assert(hwnd);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		assert(SetForegroundWindow(hwnd));*/

		// do {
			// Check for work to be performed
			// std::printf("Testing 123");
		auto task = TaskInfo(shutdown_, cond_var_, mutex_, 5000, 2000);

		/*assert(inventory_controller_.Open(Inventory::Other, task));
		
		std::this_thread::sleep_for(std::chrono::seconds(2));

		assert(inventory_controller_.Close(Inventory::Other, task));*/

		suicide_controller_.Suicide(task);

		std::unique_lock<std::mutex> lock(mutex_);
		// Sleep this thread before iterating again
		cond_var_.wait_for(lock, std::chrono::milliseconds(100));
		// } while (!shutdown_);
	});
}

void AutonomousWorker::Stop() {
	// Prevent extra calls
	if (shutdown_)
		return;

	shutdown_ = true;
	std::lock_guard<std::mutex> gaurd(mutex_);	
	cond_var_.notify_all();
}
