#ifndef DARKS_CONTROLLERS_PROCESSCONTROLLER_H_
#define DARKS_CONTROLLERS_PROCESSCONTROLLER_H_

#include "../Framework.h"

#include <tlhelp32.h>
#include <shellapi.h>
#include <processthreadsapi.h>

#include "../Log.h"
#include "../io/Pixel.h"
#include "../SyncInfo.h"
#include "MouseController.h"
#include "IDisplayCtrlPanel.h"

namespace Darks::Controller {
	class ProcessConfig {
	public:
		const wchar_t* process_name_ = L"ArkAscended.exe";
		const std::string launch_str_ = "steam://rungameid/2399830";

		DWORD wait_for_process_closed_timeout_ = 30000; // 30 seconds
	};

	class ProcessController : public IDisplayCtrlPanel {
	public:
		ProcessController(ProcessConfig conf);

		void Terminate() const;

		inline bool Launch() const {
			// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutea
			return reinterpret_cast<INT_PTR>(ShellExecuteA(NULL, "open", "C:\\Users\\Chase Roth\\Desktop\\ARK Survival Ascended.url", NULL, NULL, SW_NORMAL)) > 32; // Value above 32 == success
		}


		/// <summary>
		/// Checks to see if the process of interest is running.
		/// </summary>
		/// <param name="proc_id">Optional process id of the target if it exists.</param>
		/// <returns></returns>
		bool IsRunning(DWORD* proc_id = nullptr) const;

		void DisplayCtrlPanel() override;

	private:
		ProcessConfig conf_;

		MouseController mouse_controller_{};
	};
}

#endif