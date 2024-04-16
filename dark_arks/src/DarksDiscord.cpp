#include "DarksDiscord.h"

DarksDiscord::DarksDiscord(
	DarksDiscordConf conf,
	TribeLogController& tribe_log_controller,
	Darks::Controllers::Island::LootCrateFarmerController& loot_crate_farmer_controller
) :
	conf_(conf),
	tribe_log_controller_(tribe_log_controller),
	loot_crate_farmer_controller_(loot_crate_farmer_controller)
{
	// Listen for new tribe logs to be posted
	tribe_log_controller_.on_log_ = [this](std::unique_ptr<std::vector<char>> jpg_buf) {
		dpp::message msg;
		msg.set_channel_id(conf_.log_channel_id_);
		msg.set_guild_id(conf_.guild_id_);

		// Add the screenshot from memory to the message
		msg.add_file("logs.jpg", std::string(jpg_buf->begin(), jpg_buf->end()));

		// Ensure the client has been initialized by this point
		if (!client_) {
			DARKS_WARN("Discord client not initialized yet!");
			return;
		}

		client_->message_create(msg, [](const dpp::confirmation_callback_t e) {
			if (e.is_error()) {				
				DARKS_ERROR("Failed to send automatic tribe log screenshot.");
			}
		});
	};

	// Listen for new loot crate inventory screenshots to be posted
	loot_crate_farmer_controller_.on_crate_ = [this](const std::string& name, std::vector<char>& jpg_buf) {
		dpp::message msg;
		msg.set_channel_id(conf_.loot_crates_channel_id_);
		msg.set_guild_id(conf_.guild_id_);
		msg.content = std::format("*{}*", name);

		// Add the screenshot from memory to the message
		msg.add_file("drop.jpg", std::string(jpg_buf.begin(), jpg_buf.end()));

		// Ensure the client has been initialized by this point
		if (!client_) {
			DARKS_WARN("Discord client not initialized yet!");
			return;
		}

		client_->message_create(msg, [](const dpp::confirmation_callback_t e) {
			if (e.is_error()) {
				DARKS_ERROR("Failed to send automatic loot crate inventory screenshot.");
			}
		});
	};
}

void DarksDiscord::Start() {
	// Start the discord bot thread
	thread_ = std::thread([this]() {
		// Create discord bot
		client_ = std::make_unique<dpp::cluster>(conf_.BOT_TOKEN_);

		client_->on_slashcommand([this](const dpp::slashcommand_t& e) {
			// Request tribe log and receive image
			if (e.command.get_command_name() == "logs") {
				auto jpg_buf = tribe_log_controller_.GetScreenshot();
				
				dpp::message msg;
				
				msg.set_guild_id(conf_.guild_id_);
				msg.set_channel_id(conf_.request_channel_id_);
				msg.add_file("log.jpg", std::string(jpg_buf->begin(), jpg_buf->end()));				

				// Send an image of tribe log to requestee
				client_->direct_message_create(e.command.member.user_id, msg, [this, &e](const dpp::confirmation_callback_t& jpg_res) {
					if (jpg_res.is_error()) {
						DARKS_WARN("Failed to send image of tribe log on request.");
						return;
					}
				});

				e.reply("*sent*");
			}
		});

		client_->on_ready([this](const dpp::ready_t& e) {
			if (dpp::run_once<struct register_bot_commands>()) {
				// this->client_->global_command_create(dpp::slashcommand("logs", "Receive update-to-date logs.", client_->me.id));

				// Register guild specific command for log fetching
				this->client_->guild_command_create(dpp::slashcommand("logs", "Fetch the current logs.", client_->me.id), conf_.guild_id_);
			}

			/*dpp::message msg;
			msg.set_channel_id(conf_.state_update_channel_id_);
			msg.set_guild_id(conf_.guild_id_);
			msg.content = "Running";

 			client_->message_create(msg);*/
		});

		client_->start(dpp::st_wait);
	});
}
