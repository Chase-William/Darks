#include <memory>
#include <format>

#include "imgui.h"

#include "LoginWindow.h"

namespace Darks::UI {	

	LoginWindow::LoginWindow(
		ServiceContext& service_ctx
	) :
		DarksImguiWindow("Login"),
		service_ctx_(service_ctx)		
	{ }

	void LoginWindow::Update() {
		ImGui::Text("Please login to use this app.");
		ImGui::InputText("Token", user_token_str_, IM_ARRAYSIZE(user_token_str_));
		// ImGui::InputText("Discord-Bot-Token (Optional)", discord_bot_token_str_, IM_ARRAYSIZE(discord_bot_token_str_));

		// Login btn
		if (ImGui::Button("Login")) {
			IsVisible = false;
			service_ctx_.Login(
				user_token_str_,
				[this](std::optional<std::unique_ptr<RequestErrorMessage>> msg) {
					// If an error occured, our request error message structure will contain a value
					if (msg.has_value()) {
						// Take ownership of error
						error_ = std::move(msg.value());
						// Display login window again because login failed
						IsVisible = true;
						return;
					}
					// Clean up last error if not longer relevent
					if (error_)
						error_ = nullptr;
				});
				// If a discord bot token wasn't provided, provide empty optional
				// discord_bot_token_str_ == "" ? std::nullopt : std::optional(discord_bot_token_str_));
		}

		// If an error occured, display that error
		if (error_) {
			ImGui::Text(std::format("Error Code: {}", error_->code).c_str());
			ImGui::Text(std::format("Error Name: {}", error_->name).c_str());
			ImGui::Text(error_->description.c_str());
		}
	}
}
