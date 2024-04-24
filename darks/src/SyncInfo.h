#ifndef DARKS_SYNCINFO_H_
#define DARKS_SYNCINFO_H_

#include <string>
#include <condition_variable>
#include <functional>

#include "Log.h"

namespace Darks {
	class ManualShutdownException : public std::exception {
	public:
		const char* what() {
			return "User request shutdown of current thread.";
		}
	};

	class AutonomousResetRequiredException : public std::exception {
	public:
		AutonomousResetRequiredException(
			const char* msg
		) :
			msg_(msg)
		{
			msg_ = std::format("The player operating in autonomous mode reach a deadlock in state. Message from source: {}", msg_).c_str();
		}

		const char* what() {
			return msg_;
		}

	private:
		const char* msg_;
	};

	/// <summary>
	/// Implement to sleep a thread with interval based in-game state checks and manual shutdown checks.
	/// </summary>
	class ICheckable {
	public:
		virtual bool Check(int check_code = 0) const = 0;
	};

	struct SyncInfo {
	public:
		SyncInfo(
			std::atomic<bool>& shutdown,
			std::condition_variable& cond_var,
			std::mutex& mut
		) :
			shutdown(shutdown),
			cond_var(cond_var),
			mut(mut)
		{ }

		std::atomic<bool>& shutdown;
		std::condition_variable& cond_var;
		std::mutex& mut;

		void Wait(int milliseconds) const;

		/// <summary>
		/// Sleep in intervals as to periodically check for user defined state updates, manual shutdown requests, and timeouts.
		/// </summary>
		/// <param name="checkable">Provides the polled checking functionality.</param>
		/// <returns>Successful if the check returned true before timeout, false otherwise.</returns>
		bool Wait(const ICheckable& checkable, int poll_interval, int timeout, const int check_code = 0) const;
	};
}

#endif