#include "SyncInfo.h"

void SyncInfo::Wait(int milliseconds) const {
	// If milliseconds is 0, return without setting up delay
	if (milliseconds == 0)
		return;

	std::unique_lock<std::mutex> lock(mut);
	this->cond_var.wait_for(lock, std::chrono::milliseconds(milliseconds));
	if (this->shutdown) {
		DARKS_INFO("Initiating manual shutdown.");
		throw ManualShutdownException();
	}
}

bool SyncInfo::Wait(const ICheckable& checkable, int poll_interval, int timeout, const int check_code) const {
	// Poll until either timeout or death screen
	for (int accum_timeout = 0; accum_timeout < timeout; accum_timeout += poll_interval) {
		std::unique_lock<std::mutex> lock(this->mut);
		cond_var.wait_for(lock, std::chrono::milliseconds(poll_interval));

		if (shutdown) {
			DARKS_INFO("Shutdown initiated.");
			// User wants to terminate, should abort ASAP
			// This should be caught by callers and handled appropriately
			throw ManualShutdownException();
		}

		// Invoke check method returning a bool
		if (checkable.Check(check_code))
			return true;
	}
	// This function timed out
	return false;
}