#include <d3d11.h>
#include <tchar.h>
#include <dwmapi.h>
#include <format>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "DarkArksApp.h"
#include "OverlayWindowBase.h"
#include "controllers/frontend/AutoClickerController.h"
#include "controllers/frontend/AutoWalkerController.h"

#include "controllers/backend/MouseController.h"

#include "GlobalHotKeyManager.h"
#include "GlobalKeyboardListener.h"

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show)
{
   /* auto result1 = ParseKeyStr("2");
    auto result2 = ParseKeyStr("5");
    auto result3 = ParseKeyStr("\\");
    auto result4 = ParseKeyStr("`");*/

    GlobalHotKeyManager global_hotkey_manager = GlobalHotKeyManager();
    GlobalKeyboardListener global_keyboard_listener = GlobalKeyboardListener();

    // Library Controllers
    MouseController mc = MouseController::New();
    KeyboardController kc = KeyboardController();
    
    // Config
    DisplayControllerConfig display_conf = DisplayControllerConfig();
    AutoClickerConfig clicker_conf = AutoClickerConfig();
    AutoWalkerConfig walker_conf = AutoWalkerConfig();

    // Create controllers
    auto display_controller = DisplayController(display_conf, global_hotkey_manager);
    auto auto_clicker = AutoClickerController(clicker_conf, mc, global_hotkey_manager);
    auto auto_walker = AutoWalkerController(walker_conf, kc, global_hotkey_manager, global_keyboard_listener);
    
    auto_walker.Register();
    auto_clicker.Register();
    display_controller.Register();

    //
    //
    // Somewhere before we present the UI, we should make calls to our service to get config details.
    // Then allow each controller to perform it's setup (i.g. registering their hotkeys).
    // Once all this is done, continue.
    //
    // We may be able to allow each controller to perform it's own request at the same time.. parallelism
    // Parallel groups basically working their way up the dependency chain await all of that group to finish before continueing.
    // 
    //

    // Make app state
    auto state = DarkArksApp({ &auto_clicker, &auto_walker }, display_controller);   

    // Give state to overlay window base
    OverlayWindowBase app = OverlayWindowBase(&state, global_hotkey_manager);

    // Run the app
    try {
        app.Run();
    }
    catch (std::exception ex) {
        auto msg = ex.what();
        printf(ex.what());
    }

    auto_walker.Unregister();
    auto_clicker.Unregister();
    display_controller.Unregister();    

    return 0;
}