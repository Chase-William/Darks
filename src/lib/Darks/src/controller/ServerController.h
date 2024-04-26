#ifndef DARKS_CONTROLLERS_SERVERCONTROLLER_H_
#define DARKS_CONTROLLERS_SERVERCONTROLLER_H_

#include <string>
#include <thread>

#include "../Log.h"
#include "../io/Pixel.h"
#include "../io/Screen.h"
#include "../io/Window.h"
#include "../SyncInfo.h"
#include "MouseController.h"
#include "KeyboardController.h"
#include "IDisplayCtrlPanel.h"

namespace Darks::Controller {
	class ServerConfig {
	public:
		IO::Key console_toggle_ = IO::Key::Tab;

		IO::Point join_last_session_btn_ = { 1280, 1270 };
		IO::Point join_btn_ = { 2285, 1260 };
		IO::Pixel popup_join_btn_ = IO::Pixel({ 525, 1250 }, IO::Color(135, 79, 23)); // Btn not hovered color
		IO::Point searchbar_pos_ = { 2200, 260 };
		IO::Pixel select_server_pixel_ = IO::Pixel({ 2160, 430 }, IO::Color(83, 39, 1));
		IO::Pixel disconnect_btn_pixel = IO::Pixel({ 1178, 843 }, IO::Color(134, 234, 255));

		int select_server_attempts_ = 5;
		int join_btn_attempts_ = 5;
		int popup_join_attempts_ = 5;

		static const int server_id_max_size_ = 5;
		char target_server_id_[server_id_max_size_] = { '2', '1', '3', '4', '\0' };
		bool join_last_server_ = true;
	};

	class ServerController : public IDisplayCtrlPanel {
	public:
		ServerController(ServerConfig conf);

		/// <summary>
		/// Join a specific server. Expects the initial splash screen.
		/// </summary>
		/// <param name="info"></param>
		/// <param name="server_id"></param>
		void Join(SyncInfo& info, std::string server_id) const;

		/// <summary>
		/// Join the last played server. Expects the initial splash screen.
		/// </summary>
		/// <param name="info"></param>
		inline void JoinLast(SyncInfo& info) const {
			DARKS_INFO("Attempting to join the last played server.");
			mouse_controller_.Click(conf_.join_last_session_btn_);
			info.Wait(7500);
			ClickJoinButtons(info);
		}

		/// <summary>
		///	Disconnects the player from the current server, returning them to the main menu. This method uses the UI as the console is only available if the user is not in any kind of user interface.
		/// </summary>
		inline void Disconnect(SyncInfo& info) const {
			DARKS_INFO("Attempting to disconnect.");
			keyboard_controller_.Keystroke(IO::Key::Escape);
			info.Wait(5000);
			if (IsDisconnectButtonVisible()) {
				mouse_controller_.Click();
			}
			else {
				DARKS_ERROR("Failed to receive the in-game menu containing the disconnect button.");
			}

			// Open the console window
			/*keyboard_controller_.Keystroke(conf_.console_toggle_);
			Sleep(50);
			keyboard_controller_.Keystrokes("DISCONNECT");
			Sleep(50);
			keyboard_controller_.Keystroke(IO::Key::Enter);*/
		}

		inline bool IsSelected() const {
			return IO::Screen::GetPixelColor(conf_.select_server_pixel_.pos) == conf_.select_server_pixel_.color;
		}

		inline bool IsDisconnectButtonVisible() const {
			return IO::Screen::GetPixelColor(conf_.disconnect_btn_pixel.pos) == conf_.disconnect_btn_pixel.color;
		}

		void DisplayCtrlPanel() override;

		/// <summary>
		/// Attempts to join either the specified server, or the last server.
		/// </summary>
		/// <param name="info">Threading sync info object.</param>
		/// <param name="server_id">A specific server to be joined.</param>
		/// <param name="join_last">A toggle to override server_id and instead, join the last joined server.</param>
		void RunOnce(SyncInfo* info = nullptr, std::string server_id = "", bool join_last = true) {
			// If info is defined, run on the current thread
			if (info) {
				Run(*info, server_id, join_last);
				return;
			}

			// If was not defined, create a thread and run it
			run_once_thread_ = std::thread([this, server_id, join_last]() {
				if (!IO::Window::FocusWindow()) {
					DARKS_WARN("Failed to focus window when attempting to join the server in manual mode.");
					return;
				}
				SyncInfo info = SyncInfo(run_once_shutdown_, run_once_cond_var_, run_once_mutex_);
				Run(info, server_id, join_last);
			});
		}

	private:
		ServerConfig conf_;
		MouseController mouse_controller_{};
		KeyboardController keyboard_controller_{};

		bool running_ = false;

		// RunOnce info
		std::thread run_once_thread_;
		std::atomic<bool> run_once_shutdown_;
		std::condition_variable run_once_cond_var_{};
		std::mutex run_once_mutex_;

		/// <summary>
		/// Handles clicking the join buttons assuming a server is already selected.
		/// </summary>
		/// <param name="info"></param>
		void ClickJoinButtons(SyncInfo& info) const;

		inline bool IsPopupPresent() const {
			return IO::Screen::GetPixelColor(conf_.popup_join_btn_.pos) == conf_.popup_join_btn_.color;
		}

		inline void Run(SyncInfo& info, std::string server_id = "", bool join_last = false) const {
			if (join_last)
				JoinLast(info);
			else {
				// server id must not be empty when specifying a server to join
				if (server_id.empty()) {
					auto msg = "Attempt to join server failed as the server_id argument was found empty.";
					DARKS_ERROR(msg);
					throw std::invalid_argument(msg);
				}
				Join(info, server_id);
			}
		}
	};
}

#endif