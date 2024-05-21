#ifndef DARKS_CONTROLLER_CAMERADIRECTION_H_
#define DARKS_CONTROLLER_CAMERADIRECTION_H_

#include <array>
#include <string>

enum class CameraDirection {
	Left = 0,
	Up,
	Right,
	Down,
};

static const std::array<std::string, 4> CAMERA_DIRECTION = {
	"Left",
	"Up",
	"Right",
	"Down"
};

#endif