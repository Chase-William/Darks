#pragma once

#include <condition_variable>
#include <expected>

#include "../Log.h"
#include "../io/Pixel.h"
#include "InventoryController.h"
#include "KeyboardController.h"
#include "MouseController.h"

class SuicideConfig {
public:
	Point implant_pos_ = { 300, 370 };
	int implant_ready_delay_ = 7000;
};

class SuicideController {
public:
	SuicideController(
		SuicideConfig conf,
		InventoryController& invent_controller, 
		KeyboardController& keyboard_controller,
		MouseController& mouse_controller);

	void Suicide(TaskInfo info);
private:
	SuicideConfig conf_;
	InventoryController& invent_controller_;
	KeyboardController& keyboard_controller_;
	MouseController& mouse_controller_;
};

