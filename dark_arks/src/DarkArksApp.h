#pragma once

#include <Windows.h>
#include <vector>

#include "imgui.h"

#include "controllers/IDisplayCtrlPanel.h"
#include "controllers/IDisplayHUD.h"
#include "controllers/DisplayController.h"

class DarkArksApp
{
public:
	DarkArksApp(std::vector<IDisplayCtrlPanel*> controllers, DisplayController& display_controller);

	/// <summary>
	/// Called once for setup.
	/// </summary>
	void Init(HWND hwnd, ImFont* hud_font);

	/// <summary>
	/// Called whenever rendering is going to occur.
	/// </summary>
	/// <param name="io"></param>
	void Update();

private:
	std::vector<IDisplayCtrlPanel*> controllers_;
	DisplayController& display_controller_;
	HWND hwnd_ = NULL;
	ImFont* hud_font_;

	void RenderControlPanel();
	void RenderHUD();
};

