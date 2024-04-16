#ifndef DARKS_CONTROLLERS_TRIBELOGCONTROLLER_H_
#define DARKS_CONTROLLERS_TRIBELOGCONTROLLER_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <functional>

#include "IDisplayCtrlPanel.h"
#include "../SyncInfo.h"
#include "../io/VirtualInput.h"
#include "../io/Pixel.h"
#include "../io/Screen.h"
#include "KeyboardController.h"
#include "../GlobalTimerManager.h"

class TibeLogConfig {
public:
	Key toggle_tribe_log_key_ = Key::L;

	Pixel tribe_log_open_pixel_ = Pixel({ 1065, 213 }, Color(193, 245, 255));

	Rect tribe_log_screenshot_rect_ = Rect(1040, 285, 1515, 1070);

	int tribe_log_poll_interval_ = 60000;
};

class TribeLogController : public ICheckable, public IDisplayCtrlPanel {
public:
	TribeLogController(
		TibeLogConfig conf
	) : 
		conf_(conf)
	{ }

	bool OpenTribeLog(
		SyncInfo& info,
		int wait_for_tribe_log_open_poll_interval,
		int wait_for_tribe_log_open_timeout
	) const;
	bool OpenTribeLog(SyncInfo& info) const {
		return OpenTribeLog(info, 500, 10000);
	}

	bool CloseTribeLog(
		SyncInfo& info,
		int wait_for_tribe_log_close_poll_interval,
		int wait_for_tribe_log_close_timeout
	) const;
	bool CloseTribeLog(SyncInfo& info) const {
		return CloseTribeLog(info, 500, 10000);
	}

	bool IsTribeLogOpen() const { return Screen::GetPixelColor(conf_.tribe_log_open_pixel_.pos) == conf_.tribe_log_open_pixel_.color; }

	std::unique_ptr<std::vector<char>> GetScreenshot() const {		
		return Screen::GetScreenshot(conf_.tribe_log_screenshot_rect_);
	}

	void DisplayCtrlPanel() override;

	bool Check(int check_code = 0) const override {
		switch (check_code) {
		// Case for checking if tribe log has been opened
		case 0:
			return IsTribeLogOpen();
		// Case for checking if tribe log has been closed
		case 1:
			return !IsTribeLogOpen();
		}
	}

	bool StartPollingTribeLogs(GlobalTimerManager& timer_manager);
	bool StopPollingTribeLogs(GlobalTimerManager& timer_manager);

	/// <summary>
	/// Fires whenever a new screenshot of tribe log has been captured automatically.
	/// </summary>
	std::function<void(std::unique_ptr<std::vector<char>> jpg_buf)> on_log_;

private:
	const int WAIT_UNTIL_TRIBE_LOG_CLOSED_CODE = 10000;

	TibeLogConfig conf_;
	KeyboardController keyboard_controller_{};

	int timer_id_ = 0;
};

#endif