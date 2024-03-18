#pragma once

#include <condition_variable>

struct TaskInfo {
public:
	TaskInfo(
		std::atomic<bool>& shutdown,
		std::condition_variable& cond_var,
		std::mutex& mut,
		int timeout,
		int interval) :
		shutdown(shutdown),
		cond_var(cond_var),
		mut(mut),
		timeout(timeout),
		interval(interval)
	{ }
	
	std::atomic<bool>& shutdown;
	std::condition_variable& cond_var;
	std::mutex& mut;
	int timeout;
	int interval;
};