#include "DisplayController.h"

DisplayController::DisplayController(DisplayControllerConfig& conf, GlobalHotKeyManager& global_hotkey_manager) :
    conf_(conf),
    global_hotkey_manager_(global_hotkey_manager)
{ }

bool DisplayController::Register()
{
    // Parse hotkey_ string representation into a Key enum
    auto optional_key = ParseKeyStr(conf_.hotkey_);
    if (optional_key.has_value()) {
        auto optional_id = global_hotkey_manager_.Register(
            [this]() {
                // Toggle show on hotkey_ is clicked
                show_ = !show_;
            }, 
            optional_key.value());
        if (optional_id.has_value()) {
            hotkey_id_ = optional_id.value();
            return true;
        }
    }
    return false;
}

bool DisplayController::Unregister()
{
    // Should never attempt to unregister an alreayd unregistered hotkey_
    assert(!hotkey_id_);

    if (global_hotkey_manager_.Unregister(hotkey_id_)) {
        return !(hotkey_id_ = 0);
    }
    return false;
}

bool DisplayController::Show()
{
    return show_;
}
