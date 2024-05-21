// Darks.cpp : Defines the functions for the static library.
////
//
#include "Log.h"
//#include "DarksApp.h"
//#include "OverlayWindowBase.h"
//
//void Init()
//{
//    // Init our logger(s)
//    Darks::Log::Init();
//
//    // Log macros
//    DARKS_TRACE("Test Message");
//    DARKS_INFO("Test Message");
//    DARKS_WARN("Test Message");
//    DARKS_ERROR("Test Message");
//    DARKS_CRITICAL("Test Message");
//
//    // Creates a repeating timer that elapses every 5 seconds
//    //auto timer_manager = GlobalTimerManager();
//    //auto id = timer_manager.Register(
//    //    []() {
//    //        std::printf("asdd"); // Enqueue work to autonomous worker thread here
//    //    }, 
//    //    5000);
//
//    //
//    //
//    // Somewhere before we present the UI, we should make calls to our service to get config details.
//    // Then allow each controller to perform it's setup (i.g. registering their hotkeys).
//    // Once all this is done, continue.
//    //
//    // We may be able to allow each controller to perform it's own request at the same time.. parallelism
//    // Parallel groups basically working their way up the dependency chain await all of that group to finish before continueing.
//    // 
//    //
//
//    // Make app state
//    auto state = Darks::DarkArksApp();
//
//    // Give state to overlay window base
//    auto app = Darks::OverlayWindowBase(state);
//
//    // Start the app
//    try {
//        app.Start();
//    }
//    catch (std::exception ex) {
//        auto msg = ex.what();
//        printf(ex.what());
//    }
//
//    /*  auto_walker.Unregister();
//      auto_clicker.Unregister();
//      display_controller.Unregister();  */
//
//      // Required for visual studio
//    spdlog::drop_all();
//}