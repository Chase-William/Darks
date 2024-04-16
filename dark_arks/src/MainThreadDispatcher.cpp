#include "MainThreadDispatcher.h"

MainThreadDispatcher::MainThreadDispatcher(
    HWND main_window_handle
) :
    main_window_handle_(main_window_handle)
{ }

bool MainThreadDispatcher::Dispatch(std::function<bool()> func) {
    ++id_;
    if (functions_.insert({ id_, func }).second) {
        if (SendMessageW(main_window_handle_, id_, NULL, NULL) == TRUE) {
            if (functions_.erase(id_) == 1) {
                return true;
            }
        }        
    }
    return false;
}

bool MainThreadDispatcher::Handle(int code) const {
    if (auto handler = functions_.find(code); handler != functions_.end()) {
        // Fire the handler
        return handler->second();
    }
    return false;
}
