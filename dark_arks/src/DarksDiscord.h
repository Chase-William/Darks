#ifndef DARKS_DARKSDISCORD_H_
#define DARKS_DARKSDISCORD_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <string>
#include <thread>
#include <vector>

#include "controllers/TribeLogController.h"
#include "controllers/island/LootCrateFarmerController.h"

class DarksDiscordConf {
public:
	const std::string BOT_TOKEN_ = "MTIyMzI2MzM1NTk1NTk3MDE3MA.GE2mvf.kPjfO6ilPxUjVKg-_-7kbqqIAMqp4traYtK8Ro";

	const dpp::snowflake guild_id_ = dpp::snowflake("1099409972728700958");
	const dpp::snowflake log_channel_id_ = dpp::snowflake("1225581992888766494");
	const dpp::snowflake state_update_channel_id_ = dpp::snowflake("1225587025537929228");
	const dpp::snowflake request_channel_id_ = dpp::snowflake("1225589500827271168");
	const dpp::snowflake loot_crates_channel_id_ = dpp::snowflake("1226633191561891922");
}; 

class DarksDiscord {
public:
	DarksDiscord(
		DarksDiscordConf conf,
		TribeLogController& tribe_log_controller,
		Darks::Controllers::Island::LootCrateFarmerController& loot_crate_farmer_controller
	);

	void Start();

	inline dpp::cluster* GetClient() { return &*client_; }

private:
	DarksDiscordConf conf_;
	std::unique_ptr<dpp::cluster> client_;
	std::thread thread_;

	TribeLogController& tribe_log_controller_;
	Darks::Controllers::Island::LootCrateFarmerController& loot_crate_farmer_controller_;
};

#endif