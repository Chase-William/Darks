#include <d3d11.h>
#include <tchar.h>
#include <dwmapi.h>
#include <format>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "Log.h"

#include "DarkArksApp.h"
#include "OverlayWindowBase.h"

#include "io/GlobalHotKeyManager.h"
#include "io/GlobalKeyListener.h"

#include "io/HotKey.h"

#include "AutonomousWorker.h"

//struct Foo
//{
//    class {
//        int value;
//    public:
//        int& operator = (const int& i) { return value = i; }
//        operator int() const { return value; }
//        void TestFunc() { }
//    } alpha;
//
//    class {
//        float value;
//    public:
//        float& operator = (const float& f) { return value = f; }
//        operator float() const { return value; }
//    } bravo;
//};

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show)
{           
    // Init our logger(s)
    Darks::Log::Init();

    // Log macros
    DARKS_TRACE("Test Message");
    DARKS_INFO("Test Message");    
    DARKS_WARN("Test Message");
    DARKS_ERROR("Test Message");
    DARKS_CRITICAL("Test Message");   

    
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
    auto state = DarkArksApp();

    // Give state to overlay window base
    OverlayWindowBase app = OverlayWindowBase(state);

    // Start the app
    try {
        app.Start();
    }
    catch (std::exception ex) {
        auto msg = ex.what();
        printf(ex.what());
    }

  /*  auto_walker.Unregister();
    auto_clicker.Unregister();
    display_controller.Unregister();  */  

    // Required for visual studio
    spdlog::drop_all();

    return 0;
}