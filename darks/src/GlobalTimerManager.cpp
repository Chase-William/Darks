#include "GlobalTimerManager.h"

namespace Darks {
	static std::map<int, std::function<void()>> callbacks_;

	void TimerProc(
		HWND hwnd,
		UINT _,
		UINT_PTR id,
		DWORD __
	) {
		// Locate a handler if one exists
		if (auto entry = callbacks_.find(id); entry != callbacks_.end()) {
			// Fire the handler
			entry->second();
			return;
		}
	}

	std::optional<int> GlobalTimerManager::Register(std::function<void()> callback, int interval) {
		assert(interval >= 50);
		if (interval < 50) {
			auto msg = std::format("An invalid argument of {} was provided for the interval when registering a global timer.", interval);
			DARKS_ERROR(msg);
			throw std::invalid_argument(msg);
		}

		// Register timer callback with interval with OS
		int id = SetTimer(NULL, 0, interval, TimerProc);
		if (id) {
			// Insert the entry
			callbacks_.insert({ id, callback });
			return id;
		}
		return std::nullopt;
	}

	bool GlobalTimerManager::Unregister(int id) {
		assert(id != 0);
		if (id == 0) {
			auto msg = std::format("An invalid id was given when attempting to unregister a timer. The id's value was {}.", id);
			DARKS_ERROR(msg);
			throw std::invalid_argument(msg);
		}

		// Unregister timer with OS using id
		auto status = KillTimer(NULL, id);
		if (!status) {
			DARKS_ERROR(std::format("Was unable to unregister timer callback {}.", id));
		}
		else {
			// Remove the callback from the list of callbacks if unregistering with the OS was successful
			callbacks_.erase(id);
		}
		return status;
	}
}