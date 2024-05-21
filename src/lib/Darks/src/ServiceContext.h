#ifndef DARKS_SERVICECONTEXT_H_
#define DARKS_SERVICECONTEXT_H_

#include "Framework.h"

#include <string>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <memory>

#include "cpr/cpr.h"

#include "Log.h"
#include "io/Resolution.h"

namespace Darks {	

	struct RequestErrorMessage {
		RequestErrorMessage() { }
		RequestErrorMessage(
			int code,
			std::string name,
			std::string description
		) :
			code(code),
			name(name),
			description(description)
		{ }

		int code = 0;
		std::string name = "";
		std::string description = "";

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(RequestErrorMessage, code, name, description)
	};

	class ServiceContext {
	public:
		ServiceContext(std::string distro_token);
		
		inline bool IsLoggedIn() const { return !!bearer_token_; }

		inline const std::string& GetBearerToken() const { return *bearer_token_; }

		inline const std::string& GetBaseUrl() const { return base_url_; }

		void Login(
			std::string access_key,
			std::function<void(std::optional<std::unique_ptr<RequestErrorMessage>> err_msg)> result_callback = NULL	
		);

		std::function<void()> on_logged_in;

	private:
		std::unique_ptr<std::string> bearer_token_ = nullptr;
		std::string base_url_ = "http://localhost:5000";
		std::string distro_token_ = "";
	};

	/// <summary>
	/// Gets a request error message object containing information about the request's error.
	/// </summary>
	static std::unique_ptr<RequestErrorMessage> GetRequestErrorMsg(cpr::Response res) {
		nlohmann::json json = nlohmann::json::parse(res.text);

		RequestErrorMessage err = json.template get<RequestErrorMessage>();
		
		return std::make_unique<RequestErrorMessage>(err);
	}
}

#endif