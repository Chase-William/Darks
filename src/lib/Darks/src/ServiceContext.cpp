#include "ServiceContext.h"

namespace Darks {
	ServiceContext::ServiceContext(
		std::string distro_token,
		std::string darks_restapi_base_url
	) : 
		distro_token_(distro_token),
		darks_restapi_base_url_(darks_restapi_base_url)
	{ }

	/// <summary>
	/// Perform a web request to login and receive a bearer token.
	/// </summary>
	/// <param name="access_key">The user's provided access key.</param>
	/// <param name="result_callback">An optional listener to receive updates on request result per this request.</param>
	void ServiceContext::Login(
		std::string access_key,
		std::function<void(std::optional<std::unique_ptr<RequestErrorMessage>> err_msg)> result_callback
	) {
		if (access_key.length() == 0) {
			auto msg = "User attempted to login using an empty access key.";
			DARKS_ERROR(msg);
			throw std::invalid_argument(msg);
		}

		HW_PROFILE_INFO hardware_profile_info;
		if (!GetCurrentHwProfile(&hardware_profile_info))
		{
			auto msg = "Failed to get user hwid, required for authentication.";
			DARKS_CRITICAL(msg);
			throw std::runtime_error(msg);
		}

		// Convert hwid wchar utf-16 to normal char utf-8
		char hwid[37];
		// Replaced trailing } character with end of string to change copy behavior
		hardware_profile_info.szHwProfileGuid[37] = '\0';
		// Copy from after leading { and before ending }
		sprintf_s(hwid, "%ws", &hardware_profile_info.szHwProfileGuid[1]);

		// Get the width and height of the screen
		int primary_screen_width = GetSystemMetrics(SM_CXSCREEN);
		int primary_screen_height = GetSystemMetrics(SM_CYSCREEN);

		IO::Resolution resolution = { primary_screen_width, primary_screen_height };
		nlohmann::json resolution_json = resolution;

		auto future_text = cpr::PostCallback(
			// Create a copy of the result_callback as the original will be cleaned up already
			[this, result_callback](cpr::Response res) {
				if (res.status_code == 0) {
					if (result_callback)
						result_callback(std::make_unique<RequestErrorMessage>(0, "", "Request timed-out."));
				}
				if (res.status_code != 200) {
					// Propogate error data

					if (result_callback)
						result_callback(GetRequestErrorMsg(res));

					return;
				}

				// Get the bearer token from json and store it in field
				nlohmann::json json = nlohmann::ordered_json::parse(res.text);
				std::string bearer_token_str = json["bearer_token"].template get<std::string>();
				bearer_token_ = std::make_unique<std::string>(bearer_token_str);

				// Inform listener of success
				if (result_callback)
					result_callback(std::nullopt);

				// Fire the on_logged_in event
				if (on_logged_in)
					on_logged_in();
			},
			cpr::Url{ GetBaseUrl() + "/" + "login" },
			cpr::Header{
				{ "Content-Type", "application/json"},
				{ "Distro-Token", distro_token_ },
				{ "Access-Key", access_key },
				{ "Hwid", hwid },
			},
			cpr::Body{
				resolution_json.dump()
			},
			cpr::Timeout{ 1000 },
			GetDefaultSSLOptions()
		);	
	}
}