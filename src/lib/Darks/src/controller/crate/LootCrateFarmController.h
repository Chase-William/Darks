#ifndef DARKS_CONTROLLER_CRATE_LOOTCRATECONTROLLER_H_
#define DARKS_CONTROLLER_CRATE_LOOTCRATECONTROLLER_H_

#include <type_traits>

#include "cpr/cpr.h"

#include "Crate.h"
#include "../../io/Window.h"
#include "../ILoadable.h"
#include "../IDisplayCtrlPanel.h"
#include "../IQueueable.h"
#include "../../io/GlobalHotKeyManager.h"
#include "../InventoryController.h"
#include "../CameraController.h"
#include "../GeneralController.h"
#include "../SpawnController.h"
#include "../SuicideController.h"
#include "../MovementController.h"
#include "station/CrateStation.h"
#include "station/DefaultCrateStation.h"
#include "navigator/INavigateable.h"
#include "navigator/SwanDiveNavigator.h"

namespace Darks::Controller::Crate {

	/// <summary>
	/// Type constraint for restricting incoming type to only INavigateable derived types.
	/// </summary>
	/*template <typename T>
	concept IAmOnlyNavigator = std::is_base_of<Navigator::INavigateable, T>::value;*/

	class LootCrateFarmConfig : public ILoadable {
	public:
		static const std::string URL_SUBDIRECTORY_NAME;

		IO::Rect screenshot_rect_ = IO::Rect(0, 0, 0, 0);
		bool run_on_startup_ = false;
		bool recurring_ = false;
		std::string post_loot_webhook_ = "";

		/// <summary>
		/// A collection of crate configurations used for farming purposes.
		/// </summary>
		std::vector<CrateConfig> crates_{};		

		inline std::string GetUrl() const override {
			return std::string(GetServiceState().GetBaseUrl() + "/" + URL_SUBDIRECTORY_NAME);
		}		

		inline void Save(
			bool run_on_startup,
			bool recurring,
			const std::string& post_loot_webhook
		) {
			nlohmann::json j{
				{ "run_on_startup", run_on_startup },
				{ "recurring", recurring },
				{ "post_loot_webhook", post_loot_webhook }				
			};

			auto res = cpr::PostCallback(
				[](cpr::Response res) {
					// -------------------------------------------------- Inform user of success or failure
					if (res.status_code == 200) {
						DARKS_INFO("Successfully updated remote with new loot crate control setup.");
					}
					else {
						DARKS_ERROR("Failed to update remote with new loot crate control setup.");
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

			// Wether successful request or not, set this as our active
			recurring_ = recurring;
			run_on_startup_ = run_on_startup;
			post_loot_webhook_ = post_loot_webhook;
		}

		inline void Save(std::vector<CrateConfig*> new_crates) {
			crates_.clear();
			// Create json object containing crates
			for (auto crate : new_crates) {
				// Construct a new crate and stores its value directly in the crates vector
				crates_.push_back(CrateConfig(*crate));
			}

			// Convert the std::vec of crates to json
			nlohmann::json j{
				{ "crates", crates_ }
			};
			auto res = cpr::PostCallback([](cpr::Response res) {
				// -------------------------------------------------- Inform user of success or failure
				if (res.status_code == 200) {
					DARKS_INFO("Successfully updated remote with new loot crate setup.");
				}
				else {
					DARKS_ERROR("Failed to update remote with new loot crate setup.");
				}
			},
			cpr::Url(GetUrl() + "/update-crates"),
			cpr::Bearer(GetServiceState().GetBearerToken()),
			cpr::Header{{ "Content-Type", "application/json"}},
			cpr::Body{
				j.dump()
			});
		}
	};

	/*static void to_json(nlohmann::json& json, const LootCrateFarmConfig& conf) {
		json = nlohmann::json({
			{ "process_name", conf. },
			{ "launch_uri_str", conf.launch_uri_str_ }
		});
	}*/

	static void from_json(const nlohmann::json& json, LootCrateFarmConfig& conf) {
		auto& machine = json.at("machine");
	
		if (!machine.empty()) {
			machine.at("post_loot_webhook").get_to(conf.post_loot_webhook_);		
			machine.at("run_on_startup").get_to(conf.run_on_startup_);		
			machine.at("recurring").get_to(conf.recurring_);		
			machine.at("crates").get_to(conf.crates_);					
		}

		auto& resolution = json.at("resolution");

		resolution.at("screenshot_rect").get_to(conf.screenshot_rect_);
	}

	class LootCrateFarmController : public IDisplayCtrlPanel, public IQueueable {
	public:
		LootCrateFarmController(
			LootCrateFarmConfig conf,
			IO::GlobalHotKeyManager& hotkey_manager,
			InventoryController& inventory_controller,
			CameraController& camera_controller,
			GeneralController& general_controller,
			SpawnController& spawn_controller,
			SuicideController& suicide_controller,
			MovementController& movement_controller
		);

		void DisplayCtrlPanel() override;

		bool StartTimer(
			GlobalTimerManager& timer_manager,
			QueueSyncInfo& queue_info
		) override;
		bool StopTimer(
			GlobalTimerManager& timer_manager
		) override;

		inline void RunOnce() {
			DARKS_INFO("LootCrateFarmController was requested by the user to run once; Starting execution.");
			
			Sleep(500); // Allow the the main thread to successfully hide the overlay before processing begins

			// Attempt to focus the ark window
			if (!Darks::IO::Window::FocusWindow()) {
				DARKS_ERROR("Failed to focus target window for Autonomous Worker. Cleaning up and exiting the autonomous worker.");				
				return;
			}

			// Click "into" the game with a right click so it will start receive our IO
			Sleep(250);
			mouse_controller_.Click(Controller::ClickType::Right);
			Sleep(250);
			
			std::thread([this]() {
				Darks::SyncInfo info = Darks::SyncInfo(run_once_shutdown_, run_once_cond_var_, run_once_mutex_);

				if (!suicide_controller_.Suicide(info)) {
					DARKS_INFO("Failed to suicide on manual request.");
					SetIsRunning(false);
					return;
				}

				Run(info);
			})
			.detach();
		}

		/// <summary>
		/// Starts at a spawn screen and processes all loot crates.
		/// </summary>
		void Run(
			SyncInfo& info,
			GlobalTimerManager* const timer_manager = nullptr,
			QueueSyncInfo* const queue_info = nullptr,
			MainThreadDispatcher* const dispatcher = nullptr
		) override;

		inline bool RunOnStartup() const override { return conf_.run_on_startup_; }
		inline bool IsRecurring() const override { return conf_.recurring_; }		
		inline void SetIsRecurring(bool value) { conf_.recurring_ = value; }

		std::vector<std::function<void()>> on_started_{};
		std::vector<std::function<void()>> on_stopped_{};

		std::function<void(std::unique_ptr<CrateMessage> msg)> on_crate_;
		
		inline bool IsRunning() const override { return is_running_; }

		inline const std::string& GetWebhook() const { return conf_.post_loot_webhook_; }
	private:
		LootCrateFarmConfig conf_;
		MouseController mouse_controller_{};
		IO::GlobalHotKeyManager& hotkey_manager_;
		InventoryController& inventory_controller_;
		CameraController& camera_controller_;
		GeneralController& general_controller_;
		SpawnController& spawn_controller_;
		SuicideController& suicide_controller_;
		MovementController& movement_controller_;

		std::vector<CrateConfig*> temp_crates_{};

		int timer_id_ = 0;
		/// <summary>
		/// Whether the user is editing their configuration currently.
		/// </summary>
		bool is_editing_crates_ = false;
		bool is_editing_controls_ = false;
		bool is_running_ = false;

		bool recurring_edit_;
		bool run_on_startup_edit_;
		std::string post_loot_webhook_edit_ = "";

		// Run once info
		std::atomic<bool> run_once_shutdown_;
		std::condition_variable run_once_cond_var_{};
		std::mutex run_once_mutex_;

		/// <summary>
		/// A collection of crates to be harvested.
		/// </summary>
		std::vector<std::unique_ptr<Station::CrateStation>> crates_;	

		inline void SetIsRunning(bool value) {
			if (is_running_ == value)
				return;
			is_running_ = value;
			if (is_running_)
				for (auto& cb : on_started_)
					cb();
			else
				for (auto& cb : on_stopped_)
					cb();
		}

		/// <summary>
		/// Creates the specificed navigator and returns a smart pointer to the generic base class.
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		static std::unique_ptr<Navigator::INavigateable> GetNavigator(CrateStationNavigatorType type) {
			switch (type) {
			case CrateStationNavigatorType::None:
				return nullptr;
			case CrateStationNavigatorType::SwanDive:
				return std::make_unique<Navigator::SwanDiveNavigator>();
			default:
				auto msg = std::format("The provided argument *type* with provided value {} is invalid.", static_cast<int>(type));
				DARKS_ERROR(msg);
				throw std::invalid_argument(msg);
			}
		}

		inline void OnCrate(std::unique_ptr<CrateMessage> msg) const {
			if (!on_crate_)
				return;

			switch (msg->code) {
			case CrateMessageType::CrateOpened:
				msg->crate_inventory_screenshot = std::move(IO::Screen::GetScreenshot(conf_.screenshot_rect_));
				on_crate_(std::move(msg));
				break;
			case CrateMessageType::DepositContainerNotFound:
				on_crate_(std::move(msg));
				break;
			default:
				auto err_msg = std::format("The crate message type of {} provided to on_crate_ does not have a proper handler.", static_cast<int>(msg->code));
				DARKS_ERROR(err_msg);
				throw std::runtime_error(err_msg);
			}
		}

		void CreateRuntimeObjects();
		void CleanupRuntimeObjects();

		inline void CleanUpTemporaries() {
			for (auto crate : temp_crates_) {
				delete crate;
			}
			temp_crates_.clear();
		}
	};
}

#endif