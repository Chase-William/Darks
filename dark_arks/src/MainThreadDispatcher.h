#ifndef DARKS_MAINTHREADDISPATCHER_H_
#define DARKS_MAINTHREADDISPATCHER_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <map>
#include <functional>
#include <optional>

/// <summary>
/// Allows arbitrary code to be register/unregistered and ran on the main thread when dispatched via associated code. Does not support concurrency.
/// </summary>
class MainThreadDispatcher {
public:
	MainThreadDispatcher(HWND main_window_handle);
	
	
	bool Dispatch(std::function<bool()> func);
	bool Handle(int code) const;

private:
	class {
	public:
		operator int() const { return value; }
		int operator ++() { 
			if (value >= WM_APP) {
				return value = WM_USER;
			}
			else {
				return value++;
			}
		}
	private:
		int value = WM_USER;
	} id_;

	HWND main_window_handle_;

	std::map<int, std::function<bool()>> functions_;
};

#endif