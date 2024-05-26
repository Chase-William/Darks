#ifndef DARKS_CONTROLLERS_IDLECONTROLLER_H_
#define DARKS_CONTROLLERS_IDLECONTROLLER_H_

#include "../SyncInfo.h"
#include "IDisplayCtrlPanel.h"
#include "SpawnController.h"
#include "TribeLogController.h"
#include "CameraController.h"
#include "GeneralController.h"
#include "../GlobalTimerManager.h"
#include "../MainThreadDispatcher.h"
#include "ILoadable.h"
#include "IQueueable.h"

namespace Darks::Controller {

	constexpr int IDLE_ERROR_UNEXPECTED_TRIBELOG_CLOSED = 1;
	constexpr const char* IDLE_ERROR_UNEXPECTED_TRIBELOG_CLOSED_MSG = "While attemping to take a picture of tribe log, it was discovered the tribe log window has unexpectedly closed.";

	class IdleConfig : public ILoadable {
	public:
		static const std::string URL_SUBDIRECTORY_NAME;

		std::string idle_bed_name_ = "";

		inline std::string GetUrl() const override {
			return std::string(GetServiceState().GetBaseUrl() + "/" + URL_SUBDIRECTORY_NAME);
		}

		inline void Save(
			const std::string& idle_bed_name
		) {
			nlohmann::json j{
				{ "idle_bed_name", idle_bed_name }
			};

			auto res = cpr::PostCallback(
				[](cpr::Response res) {
					std::printf("");
					// -------------------------------------------------- Inform user of success or failure
					if (res.status_code == 200) {
						DARKS_INFO("Successfully updated remote with new idle controller setup.");
					}
					else {
						DARKS_ERROR("Failed to update remote with new idle controller setup.");
					}
				},
				cpr::Url(GetUrl() + "/update"),
				cpr::Bearer(GetServiceState().GetBearerToken()),
				cpr::Header{ { "Content-Type", "application/json" } },
				cpr::Body{
					j.dump()
				},
				GetServiceState().GetDefaultSSLOptions()
			);

			idle_bed_name_ = idle_bed_name;
		}
	};

	static void to_json(nlohmann::json& json, const IdleConfig& conf) {
		json = nlohmann::json({
			{ "idle_bed_name", conf.idle_bed_name_ }
		});
	}

	static void from_json(const nlohmann::json& json, IdleConfig& conf) {
		json.at("idle_bed_name").get_to(conf.idle_bed_name_);
	}	

	/// <summary>
	/// Used for entering the player into an idle state.
	/// </summary>
	class IdleController : public IDisplayCtrlPanel {
	public:
		IdleController(
			IdleConfig conf,
			MainThreadDispatcher& dispatcher,
			GlobalTimerManager& timer_manager,
			SpawnController& spawn_controller,
			TribeLogController& tribe_log_controller,
			CameraController& camera_controller,
			GeneralController& general_controller
		) :
			conf_(conf),
			dispatcher_(dispatcher),
			timer_manager_(timer_manager),
			spawn_controller_(spawn_controller),
			tribe_log_controller_(tribe_log_controller),
			camera_controller_(camera_controller),
			general_controller_(general_controller)
		{ 
			idle_bed_name_edit_ = conf_.idle_bed_name_;
		}

		/// <summary>
		/// Enters the player into idle state.
		/// </summary>
		/// <param name="info"></param>
		void EnterIdle(SyncInfo& info , QueueSyncInfo& queue_sync_info) const;
		/// <summary>
		/// Exits the player from idle state.
		/// </summary>
		/// <param name="info"></param>
		void ExitIdle(SyncInfo& info) const;

		bool IsIdle() const { return is_idle_; }

		void DisplayCtrlPanel() override;

	private:
		IdleConfig conf_;
		MainThreadDispatcher& dispatcher_;
		GlobalTimerManager& timer_manager_;
		SpawnController& spawn_controller_;
		TribeLogController& tribe_log_controller_;
		CameraController& camera_controller_;
		GeneralController& general_controller_;

		bool is_idle_ = false;
		
		bool is_editing_ = false;
		std::string idle_bed_name_edit_ = "";
	};
}

#endif