#include "imgui.h"

#include "Darks.h"

#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#endif

#include "controller/BerryFarmerController.h"

int WINAPI wWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PWSTR pCmdLine, 
	int nCmdShow
) {

	// Call the builder::load to get all shits async?


	// Use results to create custom controllers

	// merge results

	// give to darksapp


	// auto berry_farmer = LXD::Controller::BerryFarmerController();

	// auto app = Darks::DarkArksApp({}, { });

	Darks::IO::Point test = Darks::IO::Point(20, 20);

	return 0;
}