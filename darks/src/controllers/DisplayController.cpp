#include "DisplayController.h"

namespace Darks::Controllers {
    DisplayController::DisplayController(
        DisplayControllerConfig conf,
        IO::GlobalHotKeyManager& hotkey_manager,
        std::function<void()> handler
    ) :
        conf_(conf),
        hotkey_manager_(hotkey_manager),
        handler_(handler)
    { }

    bool DisplayController::Register() {
        return hotkey_manager_.Register(
            conf_.hotkey_,
            handler_);
    }

    bool DisplayController::Unregister() {
        if (!conf_.hotkey_.id_)
            return true;

        return hotkey_manager_.Unregister(conf_.hotkey_);
    }

    void DisplayController::Dispose() {
        hotkey_manager_.Unregister(conf_.hotkey_);
    }
}
