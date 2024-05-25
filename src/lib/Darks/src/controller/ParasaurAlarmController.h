#ifndef DARKS_CONTROLLER_PARASAURALARMCONTROLLER_H_
#define DARKS_CONTROLLER_PARASAURALARMCONTROLLER_H_

#include <string>

#include "../io/Pixel.h"
#include "IDisplayCtrlPanel.h"
#include "ILoadable.h"

namespace Darks::Controller {
	class ParasaurAlarmConfig : public ILoadable {
	public:
		static const std::string URL_SUBDIRECTORY_NAME;

		IO::Rect alarm_img_rect_;
		std::string alarm_webhook_;
		bool enabled_ = true;		

		inline void Save(
			std::string alarm_webhook,
			bool enabled
		) {
			nlohmann::json j{
				{ "alarm_webhook", alarm_webhook },
				{ "enabled", enabled }
			};

			auto res = cpr::PostCallback([](cpr::Response res) {
				std::printf("");
				if (res.status_code == 200) {
					DARKS_INFO("Successfully updated remote with new parasaur alarm config.");
				}
				else {
					DARKS_ERROR("Failed to update remote with new parasaur alarm config.");
				}
				},
				cpr::Url(GetUrl() + "/update"),
				cpr::Bearer(GetServiceState().GetBearerToken()),
				cpr::Header{ { "Content-Type", "application/json"} },
				cpr::Body{
					j.dump()
				});

			// Update config
			alarm_webhook_ = alarm_webhook;
			enabled_ = enabled;
		}
	};

	class ParasaurAlarmController : public IDisplayCtrlPanel {
	public:
		ParasaurAlarmController(
			ParasaurAlarmConfig conf
		);

		bool IsAlarming() const {
			// Check screen if alarm is firing
			return false;
		}

		void PostAlarmScreenshot() const {
			// Post alarm to webhook
		}

		void DisplayCtrlPanel() override;

		inline bool IsEnabled() const { return conf_.enabled_; }
	private:
		ParasaurAlarmConfig conf_;

		bool is_editing_ = false;
		bool enabled_edit_ = true;
		std::string alarm_webhook_edit_ = "";		
	};
}

#endif