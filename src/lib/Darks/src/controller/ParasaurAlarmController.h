#ifndef DARKS_CONTROLLER_PARASAURALARMCONTROLLER_H_
#define DARKS_CONTROLLER_PARASAURALARMCONTROLLER_H_

#include <string>
#include <functional>
#include <vector>
#include <memory>

#include "../io/Pixel.h"
#include "IDisplayCtrlPanel.h"
#include "ILoadable.h"
#include "../io/Screen.h"

namespace Darks::Controller {
	class ParasaurAlarmConfig : public ILoadable {
	public:
		static const std::string URL_SUBDIRECTORY_NAME;

		IO::Rect alarm_screenshot_rect_ = IO::Rect(0, 0, 0, 0);
		std::string alarm_webhook_ = "";
		bool enabled_ = true;		
		IO::Color alarming_color_ = IO::Color(0, 0, 0);

		inline std::string GetUrl() const override {
			return std::string(GetServiceState().GetBaseUrl() + "/" + URL_SUBDIRECTORY_NAME);
		}

		inline void Save(
			std::string alarm_webhook,
			bool enabled
		) {
			nlohmann::json j{
				{ "alarm_webhook", alarm_webhook },
				{ "enabled", enabled }
			};

			auto res = cpr::PostCallback(
				[](cpr::Response res) {
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
				}
			);

			// Update config
			alarm_webhook_ = alarm_webhook;
			enabled_ = enabled;
		}
	};

	static void from_json(const nlohmann::json& json, ParasaurAlarmConfig& conf) {
		auto& general = json.at("general");
		general.at("alarm_color").get_to(conf.alarming_color_);

		auto& machine = json.at("machine");
		
		machine.at("enabled").get_to(conf.enabled_);
		machine.at("alarm_webhook").get_to(conf.alarm_webhook_);

		auto& resolution = json.at("resolution");
		resolution.at("alarm_screenshot_rect").get_to(conf.alarm_screenshot_rect_);
	}

	class ParasaurAlarmController : public IDisplayCtrlPanel {
	public:
		ParasaurAlarmController(
			ParasaurAlarmConfig conf
		);

		bool IsAlarming() const {
			// Find the center of the rectangle
			int center_x = (conf_.alarm_screenshot_rect_.right - conf_.alarm_screenshot_rect_.left) / 2; 
			int center_y = (conf_.alarm_screenshot_rect_.bottom - conf_.alarm_screenshot_rect_.top) / 2;

			// Get the end position of the search
			int end_x = (conf_.alarm_screenshot_rect_.right - conf_.alarm_screenshot_rect_.left) / 1.5;

			for (; center_x < end_x; center_x += 2) {
				// Return true if colors match
				if (IO::Screen::GetPixelColor({ center_x, center_y }) == conf_.alarming_color_) // -- This could have a level of variance to it
					return true;
			}

			return false;
		}

		void PostAlarmScreenshot() const {
			if (on_alarming_) {
				DARKS_INFO("Posting parasaur alarm image.");
				on_alarming_(IO::Screen::GetScreenshot(conf_.alarm_screenshot_rect_), conf_.alarm_webhook_);
			}
			else
				DARKS_WARN("PostAlarmScreenshot was called, however there are not listeners listening for alarm messages, ignoring.");
		}

		void DisplayCtrlPanel() override;

		std::function<void(std::unique_ptr<std::vector<char>> jpg_buf, const std::string& url)> on_alarming_;

		inline bool IsEnabled() const { return conf_.enabled_; }
	private:
		ParasaurAlarmConfig conf_;

		bool is_editing_ = false;
		bool enabled_edit_ = true;
		std::string alarm_webhook_edit_ = "";		
	};
}

#endif