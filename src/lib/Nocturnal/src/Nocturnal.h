#ifndef NOCTURNAL_H_
#define NOCTURNAL_H_

#include <memory>
#include <vector>

#include "dpp/dpp.h"

#include "Darks.h"


namespace Nocturnal {
	static std::unique_ptr<std::vector<Darks::Controller::IQueueable*>> Init(Darks::Supplies& supplies) {
		// Create controller collectio known as queueables
		auto queueables = std::make_unique<std::vector<Darks::Controller::IQueueable*>>();
		auto discord_client = supplies.discord_bot.GetClient();

		discord_client->on_ready([discord_client](const dpp::ready_t& ev) {
			if (dpp::run_once<struct register_bot_commands>()) {
				dpp::snowflake noct_guild_id_ = dpp::snowflake("1099409972728700958");
				dpp::slashcommand mod_crates{};
				mod_crates.description = "Enable/Disable crate harvesting for your bot.";
				mod_crates.name = "crates";
				mod_crates.add_option(
					dpp::command_option(dpp::co_string, "toggle", "Enabled/Disable crate farming.")
					.add_choice(dpp::command_option_choice("on", "true"))
					.add_choice(dpp::command_option_choice("off", "false"))
					.add_choice(dpp::command_option_choice("status", "state"))
				);
				discord_client->guild_command_create(mod_crates, noct_guild_id_, [](const dpp::confirmation_callback_t& ev) {
					if (ev.is_error()) {
						DARKS_ERROR("Failed to register crates local guild command.");
					}
					else {
						DARKS_TRACE("Successfully registered crates local guild command.");
					}
				});
			}
			});

		discord_client->on_slashcommand([supplies](const dpp::slashcommand_t& event) {
			/* Check which command they ran */
			if (event.command.get_command_name() == "crates") {
				/* Fetch a parameter value from the command parameters */

				std::string param_value = std::get<std::string>(event.get_parameter("toggle"));

				bool in_autonomous_mode = supplies.worker->get()->IsRunning();

				if (param_value == "state") {

					dpp::embed embed{};

					if (!in_autonomous_mode) {
						embed.add_field("WARNING:", "The bot is not running.");
					}

					if (supplies.loot_crate_farm_controller.IsRunning()) {
						embed.add_field("INFO:", "The bot is currently collecting drops; changes have been applied.");
					}

					embed.add_field("Toggled:", supplies.loot_crate_farm_controller.IsRecurring() ? "On" : "Off");

					dpp::message msg{};
					msg.add_embed(embed);

					event.reply(msg);
					return;
				}

				bool toggle_state = param_value == "true";

				supplies.loot_crate_farm_controller.SetIsRecurring(toggle_state);
				if (in_autonomous_mode) {
					if (toggle_state) {
						supplies.loot_crate_farm_controller.StartTimer(supplies.timer_manager, supplies.worker->get()->GetQueueSyncInfo());
						event.reply("Crate farming enabled, recur timer started.");
					}
					else {
						supplies.loot_crate_farm_controller.StopTimer(supplies.timer_manager);
						event.reply("No longer farming crates, recur timer stopped.");
					}
				}
				else {
					event.reply(std::format("{} crate farming when in autonomous mode.", toggle_state ? "Enabled" : "Disabled"));
				}
			}
		});

		return std::move(queueables);
	}
}

#endif