#pragma once

#include <exception>

class ManualShutdownException : public std::exception {
public:
	const char* what() {
		return "User request shutdown of current thread.";
	}
};