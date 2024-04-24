#include "ProcessController.h"

namespace Darks::Controllers {
	ProcessController::ProcessController(
		ProcessConfig conf
	) :
		conf_(conf)
	{ }

	void ProcessController::Terminate() const {
		DWORD id = 0;
		// Nothing to do if the process is already terminated
		if (!IsRunning(&id))
			return;
		// Get a handle to TERMINATE ONLY
		HANDLE h_proc = OpenProcess(PROCESS_TERMINATE, FALSE, id);
		if (!TerminateProcess(h_proc, NULL)) {
			DARKS_ERROR("Failed to terminate the process.");
		}

		WaitForSingleObject(h_proc, conf_.wait_for_process_closed_timeout_);
		CloseHandle(h_proc);
	}

	bool ProcessController::IsRunning(DWORD* proc_id) const {
		bool exists = false;
		PROCESSENTRY32 entry{};
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry)) // Check if first element in snapshot exist
			while (Process32Next(snapshot, &entry)) // Iterate
				if (!_wcsicmp(entry.szExeFile, conf_.process_name_)) {
					exists = true;
					// If the optional param points somewhere valid, update it's value with the found proc id
					if (proc_id)
						*proc_id = entry.th32ProcessID;
					break;
				}

		CloseHandle(snapshot);
		return exists;
	}

	void ProcessController::DisplayCtrlPanel() {
		// Allow user to manually terminate process
		if (ImGui::Button("Launch Ark")) {
			Launch();
		}
		ImGui::SameLine();
		if (ImGui::Button("Terminate Ark")) {
			Terminate();
		}
	}
}