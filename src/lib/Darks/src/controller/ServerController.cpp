#include "ServerController.h"

#include "../Framework.h"

#include "imgui.h"

namespace Darks::Controller {
	ServerController::ServerController(
		ServerConfig conf
	) :
		conf_(conf)
	{ }

	void ServerController::Join(SyncInfo& info, std::string server_id) const {
		DARKS_INFO(std::format("Attempting to join server {}.", server_id));

		// Even though we are not joining the last session
		// we can still use it to navigate to the server listing screen
		mouse_controller_.Click(conf_.join_last_session_btn_);
		info.Wait(10000);

		// Focus the searhbar
		mouse_controller_.Click(conf_.searchbar_pos_);
		info.Wait(500);
		// Sometimes the ui fails to respond to the click after the move
		// Therefore, click one more time at the current location (will be set to searchbar pos already)
		mouse_controller_.Click();
		info.Wait(1000);
		// Enter server identifer
		keyboard_controller_.Keystrokes(server_id);

		for (int select_attempts = 0; select_attempts < conf_.select_server_attempts_; select_attempts++) {
			// Attempt to select the server
			mouse_controller_.Click(conf_.select_server_pixel_.pos);
			info.Wait(500);
			if (IsSelected()) {
				ClickJoinButtons(info);
				return;
			}
			// Failed to select server, wait before iter
			info.Wait(2000);
		}

		// Was unable to select the server to join
		auto msg = std::format("Was unable to select server {} to join.", server_id);
		DARKS_WARN(msg);
		throw AutonomousResetRequiredException(msg.c_str());
	}

	void ServerController::DisplayCtrlPanel() {
		ImGui::Text("Server Controller");
		/*if (ImGui::Button("Join Server")) {
			if (IO::Window::FocusWindow()) {
				Sleep(50);				
				RunOnce();
			}
			else {
				DARKS_WARN("Failed to focus window when attempting to join via manual mode.");
			}			
		}
		ImGui::SameLine();*/
		/*if (ImGui::Button("Disconnect")) {
			if (IO::Window::FocusWindow()) {
				Sleep(50);
				Disconnect();
			} 
			else {
				DARKS_WARN("Failed to focus window when attempting to disconnect via manual mode.");
			}
		}*/
		/*ImGui::SameLine();*/
		/*ImGui::SetNextItemWidth(50);
		ImGui::InputText("Server", conf_.target_server_id_, conf_.server_id_max_size_);
		ImGui::SameLine();
		ImGui::Checkbox("Last", &conf_.join_last_server_);*/
	}

	void ServerController::ClickJoinButtons(SyncInfo& info) const {
		// Attempt to click the server listing join button multiple times
		for (int join_btn_attempts = 0; join_btn_attempts < conf_.join_btn_attempts_; join_btn_attempts++) {
			// Click join
			mouse_controller_.Click(conf_.join_btn_);
			info.Wait(7500);
			// Presentation of the popup indicates success
			if (IsPopupPresent()) {
				DARKS_INFO("Server join popup successfully presented.");
				// Attempt to click the popup's join button multiple times
				for (int popup_join_attempts = 0; popup_join_attempts < conf_.popup_join_attempts_; popup_join_attempts++) {
					// Click join
					mouse_controller_.Click(conf_.popup_join_btn_.pos);
					info.Wait(750);
					// If the popup disappears, success
					if (!IsPopupPresent()) {
						DARKS_INFO("Popup disappeared, therefore we should be joining the server.");
						return;
					}
					info.Wait(1500);
				}
				// Was unable to join the server via an issue with the popup's join button
				auto msg = "Was unable to use the popup's join button correctly to join the server.";
				DARKS_ERROR(msg);
				throw AutonomousResetRequiredException(msg);
			}

			// Was unable to join the server, was unable to receive the popup from the server listing join button being clicked
			auto msg = "Was unable to receive the popup from clicking the server listing's join button.";
			DARKS_ERROR(msg);
			throw AutonomousResetRequiredException(msg);
		}
	}

}