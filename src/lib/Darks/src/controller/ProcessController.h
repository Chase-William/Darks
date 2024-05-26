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
#include "ILoadable.h"

namespace Darks::Controller {
	class ProcessConfig : public ILoadable {
	public:
		static const std::string URL_SUBDIRECTORY_NAME;

		std::string process_name_ = "";
		std::string launch_uri_str_ = "";
		std::string short_cut_url_path_ = "";

		DWORD wait_for_process_closed_timeout_ = 30000; // 30 seconds

		inline std::string GetUrl() const override {
			return std::string(GetServiceState().GetBaseUrl() + "/" + URL_SUBDIRECTORY_NAME);
		}

		inline void Save(
			std::string short_cut_url_path
		) {
			nlohmann::json j{
				{ "short_cut_url_path", short_cut_url_path }
			};

			auto res = cpr::PostCallback([](cpr::Response res) {
				std::printf("");
				if (res.status_code == 200) {
					DARKS_INFO("Successfully updated remote with new process config.");
				}
				else {
					DARKS_ERROR("Failed to update remote with new process config.");
				}
				},
				cpr::Url(GetUrl() + "/update"),
				cpr::Bearer(GetServiceState().GetBearerToken()),
				cpr::Header{ { "Content-Type", "application/json"} },
				cpr::Body{
					j.dump()
				});

			// Update config
			short_cut_url_path_ = short_cut_url_path;
		}
	};

	/*static void to_json(nlohmann::json& json, const ProcessConfig& conf) {
		json = nlohmann::json({
			{ "process_name", conf.process_name_ },
			{ "launch_uri_str", conf.launch_uri_str_ }
		});
	}*/

	static void from_json(const nlohmann::json& json, ProcessConfig& conf) {
		auto& machine = json.at("machine");
		
		machine.at("short_cut_url_path").get_to(conf.short_cut_url_path_);

		auto& general = json.at("general");

		general.at("process_name").get_to(conf.process_name_);
		general.at("launch_uri_str").get_to(conf.launch_uri_str_);
	}

	class ProcessController : public IDisplayCtrlPanel {
	public:
		ProcessController(ProcessConfig conf);

		void Terminate() const;

		inline bool Launch() const {
			// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutea
			return reinterpret_cast<INT_PTR>(ShellExecuteA(
				NULL, 
				"open", 
				conf_.short_cut_url_path_.c_str(),
				NULL, 
				NULL, 
				SW_NORMAL)) > 32; // Value above 32 == success
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

		bool is_editing_ = false;
		std::string short_cut_url_path_edit_ = "";
	};
}

#endif