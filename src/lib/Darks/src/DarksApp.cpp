#include "DarksApp.h"

namespace Darks {
	DarksApp::DarksApp(
		std::string distro_token,
		std::optional<std::string> discord_bot_token,
		std::function<std::unique_ptr<std::vector<Controller::IQueueable*>>(Supplies& supplies)> make_queueables,
		std::string darks_restapi_base_url
	) {
		assert(distro_token.length() != 0);
		if (distro_token.length() == 0) {
			auto msg = "The provided distro token was empty.";
			DARKS_CRITICAL(msg);
			throw std::invalid_argument(msg);
		}

		// Initialize logging
		Darks::Log::Init();

		// Log macros
		DARKS_TRACE("Test Message");
		DARKS_INFO("Test Message");
		DARKS_WARN("Test Message");
		DARKS_ERROR("Test Message");
		DARKS_CRITICAL("Test Message");		

		service_state_ = std::make_unique<ServiceContext>(distro_token, darks_restapi_base_url);
		login_window_ = std::make_unique<UI::LoginWindow>(*service_state_);		

		if (discord_bot_token.has_value()) {
			DARKS_INFO("Creating discord bot using user's provided token.");
			discord_ = std::make_unique<DarksDiscord>(discord_bot_token.value()); // Create fully fledged discord bot
		}
		// For simplicity, resort to making a default bot for webhooks if a user-specific bot isn't needed
		else {
			DARKS_INFO("Creating discord bot for webhooks only.");
			discord_ = std::make_unique<DarksDiscord>(); // Create discord bot for operating webhooks
		}		

		service_state_->on_logged_in = [this, make_queueables]() {
			// -------------------------------------------------------------------- We need to perform https://docs.libcpr.org/advanced-usage.html#asynchronous-requests to get all configurations
			// Configrations then need to be used to create each controller until all are created
			// These stored in a collection			
			
			// Create a seperate thread to synchronize the batch of thread request
			std::thread([this, make_queueables]() {

				// Write up each config to statically have a pointer to the shared service state object
				Darks::Controller::MovementConfig::SetServiceState(&*service_state_);
				Darks::Controller::ProcessConfig::SetServiceState(&*service_state_);
				Darks::Controller::ServerConfig::SetServiceState(&*service_state_);
				Darks::Controller::InventoryConfig::SetServiceState(&*service_state_);
				Darks::Controller::TribeLogConfig::SetServiceState(&*service_state_);
				Darks::Controller::SpawnConfig::SetServiceState(&*service_state_);
				Darks::Controller::SuicideConfig::SetServiceState(&*service_state_);
				Darks::Controller::IdleConfig::SetServiceState(&*service_state_);
				Darks::Controller::Crate::LootCrateFarmConfig::SetServiceState(&*service_state_);

				const int MOVEMENT_CONFIG_INDEX = 0;
				const int PROCESS_CONFIG_INDEX = 1;
				const int SERVER_CONFIG_INDEX = 2;
				const int INVENTORY_CONFIG_INDEX = 3;
				const int TRIBE_LOG_CONFIG_INDEX = 4;
				const int SPAWN_CONFIG_INDEX = 5;
				const int SUICIDE_CONFIG_INDEX = 6;
				const int IDLE_CONFIG_INDEX = 7;
				const int LOOT_CRATE_CONFIG_INDEX = 8;

				// Create a batch of request to be made in parallel
				std::vector<cpr::AsyncWrapper<cpr::Response, true>> responses{				
					cpr::MultiPostAsync(						
						Controller::MovementConfig::GetLoadRequest(
							Controller::MovementConfig::URL_SUBDIRECTORY_NAME),						
						Controller::ProcessConfig::GetLoadRequest(
							Controller::ProcessConfig::URL_SUBDIRECTORY_NAME),
						Controller::ServerConfig::GetLoadRequest(
							Controller::ServerConfig::URL_SUBDIRECTORY_NAME),
						Controller::InventoryConfig::GetLoadRequest(
							Controller::InventoryConfig::URL_SUBDIRECTORY_NAME),
						Controller::TribeLogConfig::GetLoadRequest(
							Controller::TribeLogConfig::URL_SUBDIRECTORY_NAME),
						Controller::SpawnConfig::GetLoadRequest(
							Controller::SpawnConfig::URL_SUBDIRECTORY_NAME),
						Controller::SuicideConfig::GetLoadRequest(
							Controller::SuicideConfig::URL_SUBDIRECTORY_NAME),
						Controller::IdleConfig::GetLoadRequest(
							Controller::IdleConfig::URL_SUBDIRECTORY_NAME),
						Controller::Crate::LootCrateFarmConfig::GetLoadRequest(
							Controller::Crate::LootCrateFarmConfig::URL_SUBDIRECTORY_NAME))
				};

				// If the first transaction isn't completed within 10 ms, we'd like to cancel all of them
				bool all_cancelled{ false };
				if (responses.at(0).wait_for(std::chrono::milliseconds(1500)) == std::future_status::timeout) {
					all_cancelled = true;
					for (auto& res : responses) {
						all_cancelled &= (res.Cancel() == cpr::CancellationResult::success);
					}
				}

				// Iterate over all results in the same order the collection was created in
				// Using a switch with an index given for a config and its place within the collection
				const int len = responses.size();
				for (int i = 0; i < len; i++) {
					auto& res = responses[i];

					// Check if job is done
					if (res.IsCancelled()) {
						MessageBoxA(NULL, "Fetching configuration data was cancelled.", "Error", MB_OK);
						exit(0);
					}

					// Wait until this request is done
					cpr::Response result = res.get();

					// Check status code123
					if (result.status_code != 200) {
						MessageBoxA(NULL, std::format("Failed to acquire configuration data from server with: {}", result.text).c_str(), "Error", MB_OK);
						exit(0);
					}
					
					try {
						nlohmann::json json = nlohmann::json::parse(result.text);

						switch (i) {
						case MOVEMENT_CONFIG_INDEX:
							DARKS_INFO("Creating MovementController.");
							movement_controller_ = std::make_unique<Controller::MovementController>(json);
							break;
						case PROCESS_CONFIG_INDEX:
							DARKS_INFO("Creating ProcessController.");
							process_controller_ = std::make_unique<Controller::ProcessController>(json);
							break;
						case SERVER_CONFIG_INDEX:
							DARKS_INFO("Creating ServerController.");
							server_controller_ = std::make_unique<Controller::ServerController>(json);
							break;
						case INVENTORY_CONFIG_INDEX:
							DARKS_INFO("Creating InventoryController.");
							inventory_controller_ = std::make_unique<Controller::InventoryController>(json);
							break;
						case TRIBE_LOG_CONFIG_INDEX:
							DARKS_INFO("Creating TribeLogController.");
							tribe_log_controller_ = std::make_unique<Controller::TribeLogController>(json);
							break;
						case SPAWN_CONFIG_INDEX:
							DARKS_INFO("Creating SpawnController.");
							spawn_controller_ = std::make_unique<Controller::SpawnController>(json);
							break;
						case SUICIDE_CONFIG_INDEX:
							DARKS_INFO("Creating SuicideController.");
							suicide_controller_ = std::make_unique<Controller::SuicideController>(
								json,
								*spawn_controller_,
								*inventory_controller_);
							break;
						case IDLE_CONFIG_INDEX:
							DARKS_INFO("Creating IdleController.");
							idle_controller_ = std::make_unique<Controller::IdleController>(
								json,
								*dispatcher_,
								timer_manager,
								*spawn_controller_,
								*tribe_log_controller_,
								*camera_controller_,
								*general_controller_);
							break;
						case LOOT_CRATE_CONFIG_INDEX:
							DARKS_INFO("Creating LootCrateController.");
							loot_crate_farm_controller_ = std::make_unique<Controller::Crate::LootCrateFarmController>(
								json,
								hotkey_manager_,
								*inventory_controller_,
								*camera_controller_,
								*general_controller_,
								*spawn_controller_,
								*suicide_controller_,
								*movement_controller_
							);
							break;
						default:
							auto msg = std::format("Failed to match a switch statement with controller information: {}.", json.dump());
							DARKS_CRITICAL(msg);
							throw std::runtime_error(msg);
						}
					}
					catch (nlohmann::json::out_of_range ex) {
						auto msg = ex.what();
						DARKS_CRITICAL(msg);
						MessageBoxA(NULL, std::format("Failed to create a standard library controller with ex: {}", msg).c_str(), "Error", MB_OK);
						exit(0);
					}																												
				}				

				DARKS_INFO("Finished Controller Initialization.");

				// Wire up tribe-log controller to post screenshots of tribe log
				tribe_log_controller_->on_log_ = [this](std::unique_ptr<std::vector<char>> jpg_buf, const std::string& url) {
					dpp::message msg{};
					dpp::webhook wh{ url };

					// Add the screenshot from memory to the message
					msg.add_file("logs.jpg", std::string(jpg_buf->begin(), jpg_buf->end()));

					discord_->GetClient()->execute_webhook(wh, msg);
				};

				// Wire up loot crate farming for posting screenshots
				loot_crate_farm_controller_->on_crate_ = [this](std::unique_ptr<Controller::Crate::CrateMessage> crate_msg) {
					dpp::message msg{};
					msg.set_content(std::format("Crate: {}", crate_msg->bed_name));
					dpp::webhook wh{ loot_crate_farm_controller_->GetWebhook() };

					msg.add_file("loot.jpg", std::string(crate_msg->crate_inventory_screenshot->begin(), crate_msg->crate_inventory_screenshot->end()));
					discord_->GetClient()->execute_webhook(wh, msg);
				};


				// To be used when creating custom controllers
				auto supplies = Supplies{
						*general_controller_,
						*movement_controller_,
						*camera_controller_,
						*inventory_controller_,
						*suicide_controller_,
						*spawn_controller_,
						*tribe_log_controller_,
						*process_controller_,
						*server_controller_,
						*loot_crate_farm_controller_,
						hotkey_manager_,
						key_listener_,
						timer_manager,
						*dispatcher_,
						*discord_, // A reference to the location of the std::unique_ptr
						&autonomous_worker_
				};

				DARKS_INFO("Creating User-Defined Controllers.");
				std::unique_ptr<std::vector<Controller::IQueueable*>> queueables = std::move(make_queueables(supplies));

				// Add loot crate farm to queueables
				queueables->push_back(&*loot_crate_farm_controller_);

				DARKS_INFO("Creating AutonomousWorker.");
				autonomous_worker_ = std::make_unique<AutonomousWorker>(
					timer_manager,
					*dispatcher_,
					*suicide_controller_,
					*spawn_controller_,
					*idle_controller_,
					*process_controller_,
					*server_controller_,
					*tribe_log_controller_,
					std::move(queueables)
				);

				// Start the discord bot
				discord_->Start();

				DARKS_INFO("Creating the Autonomous ImGui Window");
				autonomous_window_ = std::make_unique<Darks::UI::AutonomousModeWindow>(
					*autonomous_worker_
				);				
			}).detach();

			// Used to toggle the control panel visiblity
			auto display_conf = Darks::Controller::DisplayControllerConfig();
			display_controller_ = std::make_unique<Darks::Controller::DisplayController>(
				display_conf,
				hotkey_manager_,
				[this]() {
					// ---------------------------------------------------------------------------------------------- manual visibility toggle here
				});
			// Register a callback to toggle the visiblity of imgui windows
			display_controller_->Register();
		};
	}

	void DarksApp::Run() {
		overlay_window_base_ = std::make_unique<Darks::OverlayWindowBase>();
		
		overlay_window_base_->on_init = [this](HWND hwnd, MainThreadDispatcher& dispatcher) { Init(hwnd, dispatcher); };
		overlay_window_base_->on_update = [this]() { OnUpdate(); };
		overlay_window_base_->on_hotkey_pressed = [this](int id) { OnHotKeyPressed(id); };
		overlay_window_base_->on_shutdown = [this]() { OnShutdown(); };

		overlay_window_base_->Start();
	}

	void DarksApp::Init(HWND hwnd, MainThreadDispatcher& dispatcher) {
		hwnd_ = hwnd;
		dispatcher_ = &dispatcher;
		// hud_font_ = hud_font;		

		// If the user is not already logged in, ask them to login before proceding
		if (!service_state_ || service_state_->IsLoggedIn()) {
			
		}

		auto camera_conf = Darks::Controller::CameraConfig();
		camera_controller_ = std::make_unique<Darks::Controller::CameraController>(camera_conf);

		auto general_conf = Darks::Controller::GeneralConfig();
		general_controller_ = std::make_unique<Darks::Controller::GeneralController>(general_conf);

		//// Create imgui control panel window
		UI::DarksImguiWindow::Init(hwnd);
	}

	void DarksApp::OnUpdate() {

		// --- Always make sure the user is logged in ---

		// If the user is not already logged in, ask them to login before proceding
		if (!service_state_->IsLoggedIn()) {
			// Show a window for logging in and do that whole process before proceeding			
			Darks::UI::DarksImguiWindow::Update({ &*login_window_ });
			return;
		}
		
		// Show autonomous window if it has been intialized and is not currently running (the imgui window could interfere with pixel color sampling)
		if (autonomous_worker_) {
			// Use the base class function for updating
			Darks::UI::DarksImguiWindow::Update({ &*autonomous_window_ });
		}		

		// Always show HUD... for now
		// RenderHUD();
	}

	void DarksApp::OnHotKeyPressed(int id) {
		DARKS_TRACE(fmt::format("Hotkey with id: {:d} pressed.", id));
		hotkey_manager_.Dispatch(id);
	}

	void DarksApp::OnShutdown() {
		// Perform cleanup on controller that require it		
		display_controller_->Dispose();

		spdlog::drop_all();
	}

	void DarksApp::RenderHUD() {
		// ImGui::PushFont(hud_font_); // Push HUD font to increase size of all rendered text from here until pop
		// Iterate over our controller and call their rendering methods
		/*for (Darks::Controller::IDisplayCtrlPanel* controller : *queueables_) {
			if (Darks::Controller::IDisplayHUD* overlay = dynamic_cast<Darks::Controller::IDisplayHUD*>(controller)) {
				overlay->DisplayHUD();
			}
		}*/
		// ImGui::PopFont(); // no longer display bug
	}
}