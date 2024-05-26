#include <memory>

#include "imgui.h"
#include "dpp/dpp.h"

#include "Darks.h"
#include "cpr/cpr.h"

#include "Nocturnal.h"

// #include "controller/LootCrateFarmerController.h"

const std::string DISTRO_TOKEN = "9c42cc223df2ee27a3d045503ac2072d4d0c974110d9c9aca2d9cf493ba23264";
const std::string DISCORD_BOT_TOKEN = "MTIyMzI2MzM1NTk1NTk3MDE3MA.GyDBxk.9uKkvZwSNmSA10QzC-uGG4yA5kUuYpp7tvUeE0";

std::string bearer_token;

int WINAPI wWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PWSTR pCmdLine, 
	int nCmdShow
) {
	// Create a webhook for crates
	//auto crates_wh = dpp::webhook{ "" };	
	Darks::DarksApp app = Darks::DarksApp(
		DISTRO_TOKEN,
		DISCORD_BOT_TOKEN,
		[](Darks::Supplies& supplies) {			
			auto queueables = Nocturnal::Init(supplies);			

			// Return this to Darks
			return std::move(queueables);
		},
		"https://192.168.1.137");

	app.Run();

	return 0;
}