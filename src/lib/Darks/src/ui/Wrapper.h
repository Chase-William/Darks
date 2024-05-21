#ifndef DARKS_CONTROLLER_UI_WRAPPER_H_
#define DARKS_CONTROLLER_UI_WRAPPER_H_

#include <string>

#include "imgui.h"

namespace Darks::UI {
	
	/* Wrappers for ImGui */

	// Wrapper for InputText accepting an std::string.
	IMGUI_API bool InputText(
		const char* label, 
		std::string* str, 
		ImGuiInputTextFlags flags = 0, 
		ImGuiInputTextCallback callback = nullptr, 
		void* user_data = nullptr
	);
}

#endif