#ifndef DARKS_CONTROLLERS_PROCESSCONTROLLER_H_
#define DARKS_CONTROLLERS_PROCESSCONTROLLER_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <tlhelp32.h>
#include <processthreadsapi.h>
#include <string>
#include <shellapi.h>

#include "../Log.h"
#include "../io/Pixel.h"
#include "../SyncInfo.h"
#include "MouseController.h"

class ProcessConfig {
public:
	const wchar_t* process_name_ = L"ArkAscended.exe";
	const std::string launch_str_ = "steam://rungameid/2399830";

	int wait_for_process_closed_timeout = 30000;

	Point join_last_session_btn_ = Point(1280, 1270);
	Point join_btn_ = Point(2285, 1260);
	Point join_popup_btn_ = Point(460, 1250);
};

class ProcessController {
public:
	ProcessController(ProcessConfig conf);

	void Terminate() const;

	inline bool Launch() const {
		// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutea
		return reinterpret_cast<INT_PTR>(ShellExecuteA(NULL, "open", "C:\\Users\\Chase Roth\\Desktop\\ARK Survival Ascended.url", NULL, NULL, SW_NORMAL)) > 32; // Value above 32 == success
	}

	inline void JoinServer(SyncInfo& info) const {
		mouse_controller_.Click(conf_.join_last_session_btn_);
		info.Wait(10000);
		mouse_controller_.Click(conf_.join_btn_);	
		info.Wait(10000);
		mouse_controller_.Click(conf_.join_popup_btn_);
	}

	/// <summary>
	/// Checks to see if the process of interest is running.
	/// </summary>
	/// <param name="proc_id">Optional process id of the target if it exists.</param>
	/// <returns></returns>
	bool IsRunning(DWORD* proc_id = nullptr) const;

private:
	ProcessConfig conf_;

	MouseController mouse_controller_{};
};

#endif