#ifndef DARKS_CONTROLLERS_TRIBELOGCONTROLLER_H_
#define DARKS_CONTROLLERS_TRIBELOGCONTROLLER_H_

#include "IDisplayCtrlPanel.h"
#include "../SyncInfo.h"
#include "../io/VirtualInput.h"
#include "../io/Pixel.h"
#include "../io/Screen.h"
#include "KeyboardController.h"
#include "../GlobalTimerManager.h"
#include "ILoadable.h"

namespace Darks::Controller {
	class TribeLogConfig : public ILoadable {
	public:
		static const std::string URL_SUBDIRECTORY_NAME;

		Darks::IO::Key toggle_tribe_log_key_ = IO::Key::L;
		Darks::IO::Pixel tribe_log_open_pixel_ = IO::Pixel({ 0, 0 }, IO::Color(0, 0, 0));
		Darks::IO::Rect tribe_log_screenshot_rect_ = IO::Rect(0, 0, 0, 0);

		bool enabled_ = true;

		/// <summary>
		/// The web hook to post tribe logs to; shouldn't be longer than 128 characters.
		/// </summary>
		std::string post_logs_webhook_ = "";

		inline std::string GetUrl() const override {
			return std::string(GetServiceState().GetBaseUrl() + "/" + URL_SUBDIRECTORY_NAME);
		}

		inline void Save(
			std::string post_logs_webhook,
			bool enabled
		) {
			nlohmann::json j{
				{ "post_logs_webhook", post_logs_webhook },
				{ "enabled", enabled }
			};

			auto res = cpr::PostCallback([](cpr::Response res) {
				std::printf("");
				if (res.status_code == 200) {
					DARKS_INFO("Successfully updated remote with new tribe logging config.");
				}
				else {
					DARKS_ERROR("Failed to update remote with new tribe logging config.");
				}
			},
			cpr::Url(GetUrl() + "/update"),
			cpr::Bearer(GetServiceState().GetBearerToken()),
			cpr::Header{ { "Content-Type", "application/json"} },
			cpr::Body{
				j.dump()
			});
			
			// Update config
			post_logs_webhook_ = post_logs_webhook;
			enabled_ = enabled;
		}
	};

	/*static void to_json(nlohmann::json& json, const TribeLogConfig& conf) {
		json = nlohmann::json({
			{ "toggle_tribe_log_key", conf.toggle_tribe_log_key_ },
			{ "tribe_log_open_pixel", conf.tribe_log_open_pixel_ },
			{ "tribe_log_screenshot_rect", conf.tribe_log_screenshot_rect_ }
		});
	}*/

	static void from_json(const nlohmann::json& json, TribeLogConfig& conf) {
		auto& machine = json.at("machine");

		machine.at("toggle_tribe_log_key").get_to(conf.toggle_tribe_log_key_);
		machine.at("post_logs_webhook").get_to(conf.post_logs_webhook_);

		auto& resolution = json.at("resolution");

		resolution.at("tribe_log_open_pixel").get_to(conf.tribe_log_open_pixel_);
		resolution.at("tribe_log_screenshot_rect").get_to(conf.tribe_log_screenshot_rect_);
	}

	class TribeLogController : public ICheckable, public IDisplayCtrlPanel {
	public:
		TribeLogController(
			TribeLogConfig conf
		);

		bool OpenTribeLog(
			SyncInfo& info,
			int wait_for_tribe_log_open_poll_interval,
			int wait_for_tribe_log_open_timeout
		) const;
		bool OpenTribeLog(SyncInfo& info) const {
			return OpenTribeLog(info, 500, 10000);
		}

		bool CloseTribeLog(
			SyncInfo& info,
			int wait_for_tribe_log_close_poll_interval,
			int wait_for_tribe_log_close_timeout
		) const;
		bool CloseTribeLog(SyncInfo& info) const {
			return CloseTribeLog(info, 500, 10000);
		}

		bool IsTribeLogOpen() const { return IO::Screen::GetPixelColor(conf_.tribe_log_open_pixel_.pos) == conf_.tribe_log_open_pixel_.color; }

		std::unique_ptr<std::vector<char>> GetScreenshot() const {
			return IO::Screen::GetScreenshot(conf_.tribe_log_screenshot_rect_);
		}

		void DisplayCtrlPanel() override;

		bool Check(int check_code = 0) const override {
			switch (check_code) {
				// Case for checking if tribe log has been opened
			case 0:
				return IsTribeLogOpen();
				// Case for checking if tribe log has been closed
			case 1:
				return !IsTribeLogOpen();
			}
		}

		/*bool StartPollingTribeLogs(GlobalTimerManager& timer_manager, std::function<void()> tribe_log_closed_unexpectedly_handler_);
		bool StopPollingTribeLogs(GlobalTimerManager& timer_manager);*/

		/// <summary>
		/// Fires whenever a new screenshot of tribe log has been captured automatically.
		/// </summary>
		std::function<void(std::unique_ptr<std::vector<char>> jpg_buf, const std::string& url)> on_log_;

		inline const std::string& GetWebhookUrl() const { return conf_.post_logs_webhook_; }

		inline bool IsEnabled() const { return conf_.enabled_; }

		inline void PostTribeLogs() const {
			if (this->on_log_) {
				DARKS_INFO("Taking a screenshot of tribe logs.");
				this->on_log_(this->GetScreenshot(), this->GetWebhookUrl());
			}
			else {
				DARKS_WARN("PostTribeLogs was called, however there are no subscribers listening to tribe log events, therefore capturing a screenshot of tribe log has been skipped.");
			}
		}

	private:
		const int WAIT_UNTIL_TRIBE_LOG_CLOSED_CODE = 10000;

		TribeLogConfig conf_;		
		KeyboardController keyboard_controller_{};

		bool is_editing_ = false;
		bool enabled_edit_ = true;
		std::string post_logs_edit_ = "";

		int timer_id_ = 0;
	};
}

#endif