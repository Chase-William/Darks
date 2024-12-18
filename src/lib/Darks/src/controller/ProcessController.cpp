#include "ProcessController.h"

#include "../Framework.h"

#include "imgui.h"

#include "../ui/Wrapper.h"

namespace Darks::Controller {
	const std::string ProcessConfig::URL_SUBDIRECTORY_NAME = "process";

	ProcessController::ProcessController(
		ProcessConfig conf
	) :
		conf_(conf)
	{ 
		short_cut_url_path_edit_ = conf_.short_cut_url_path_;
	}

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
				if (!_wcsicmp(
					entry.szExeFile, // Convert std::string to std::wstring for comparison
					std::wstring(conf_.process_name_.begin(), conf_.process_name_.end()).c_str())) {
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
		if (ImGui::TreeNode("Ark Process Configuration")) {
			if (is_editing_) {
				if (ImGui::Button("Save")) {
					conf_.Save(short_cut_url_path_edit_);
					is_editing_ = false;
					ImGui::TreePop();
					return;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					short_cut_url_path_edit_ = conf_.short_cut_url_path_;
					is_editing_ = false;
				}
				UI::InputText("Path to ASA Launch Shortcut", &short_cut_url_path_edit_);
			}
			else {
				if (ImGui::Button("Edit")) {
					is_editing_ = true;
				}
				ImGui::SameLine();
				ImGui::Text(std::format("Shortcut Path: {}", conf_.short_cut_url_path_).c_str());
			}

			ImGui::TreePop();
		}

		// Allow user to manually terminate process
		/*if (ImGui::Button("Launch Ark")) {
			Launch();
		}
		ImGui::SameLine();
		if (ImGui::Button("Terminate Ark")) {
			Terminate();
		}*/
	}
}