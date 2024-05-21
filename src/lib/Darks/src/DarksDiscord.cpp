#include "DarksDiscord.h"

#include <vector>

#include "Log.h"

namespace Darks {

	DarksDiscord::DarksDiscord(
		std::string token
	) :
		token_(token)
	{ 
		client_ = std::make_unique<dpp::cluster>(token_);
	}

	void DarksDiscord::Start() {
		// Start the discord bot thread
		std::thread([this]() {
			DARKS_INFO("Created separate thread running the Discord bot.");
			// Create discord bot
			// client_ = std::make_unique<dpp::cluster>(token_);

			//client_->on_slashcommand([this](const dpp::slashcommand_t& e) {
			//	DARKS_INFO("Registering slash commands.");
			//	// Request tribe log and receive image
			//	//if (e.command.get_command_name() == "logs") {
			//	//	auto jpg_buf = tribe_log_controller_.GetScreenshot();

			//	//	dpp::message msg;

			//	//	msg.set_guild_id(conf_.guild_id_);
			//	//	msg.set_channel_id(conf_.request_channel_id_);
			//	//	msg.add_file("log.jpg", std::string(jpg_buf->begin(), jpg_buf->end()));

			//	//	// Send an image of tribe log to requestee
			//	//	client_->direct_message_create(e.command.member.user_id, msg, [this, &e](const dpp::confirmation_callback_t& jpg_res) {
			//	//		if (jpg_res.is_error()) {
			//	//			DARKS_WARN("Failed to send image of tribe log on request.");
			//	//			return;
			//	//		}
			//	//		});

			//	//	e.reply("*sent*");
			//	//}
			//});
			
			//client_->on_ready([this](const dpp::ready_t& e) {
			//	DARKS_INFO("Discord bot has signaled a status of ready.");
			//	//if (dpp::run_once<struct register_bot_commands>()) {
			//		// this->client_->global_command_create(dpp::slashcommand("logs", "Receive update-to-date logs.", client_->me.id));

			//		// Register guild specific command for log fetching
			//		// this->client_->guild_command_create(dpp::slashcommand("logs", "Fetch the current logs.", client_->me.id), conf_.guild_id_);
			//	//}

			//	/*dpp::message msg;
			//	msg.set_channel_id(conf_.state_update_channel_id_);
			//	msg.set_guild_id(conf_.guild_id_);
			//	msg.content = "Running";

			//	client_->message_create(msg);*/
			//});

			client_->start(dpp::st_wait);
		}).detach();
	}
}
