#include <memory>

#include "imgui.h"
#include "dpp/dpp.h"

#include "Darks.h"
#include "cpr/cpr.h"

#include "Nocturnal.h"

const std::string DISTRO_TOKEN = "ff6e139720c4f1b766bd439a5289ed8ed6410651f3590d71666a4235b8f6add5";
const std::string DISCORD_BOT_TOKEN = "MTI0MjI2MjE5NjE2NDEwMDEwNw.G-A85a.KpHY3__4ichsg0cXDvpW0zm564zLJ3A4IQlF24";

std::string bearer_token;

int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR pCmdLine,
	int nCmdShow
) {
	Darks::DarksApp app = Darks::DarksApp(
		DISTRO_TOKEN,
		DISCORD_BOT_TOKEN,
		[](Darks::Supplies& supplies) {
			auto queueables = Nocturnal::Init(supplies);					

			// Return this to Darks
			return std::move(queueables);
		});

	app.Run();

	return 0;
}