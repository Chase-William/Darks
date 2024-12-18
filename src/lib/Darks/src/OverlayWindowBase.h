#ifndef DARKS_OVERLAYWINDOWBASE_H_
#define DARKS_OVERLAYWINDOWBASE_H_

#include "Framework.h"

#include <functional>
#include <d3d11.h>
#include <dwmapi.h>

#include <imgui_impl_dx11.h>

#include "MainThreadDispatcher.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Darks {	

	class OverlayWindowBase
	{
	public:
		/// <summary>
		/// Creates an overlay app instance.
		/// </summary>
		OverlayWindowBase();

		void Start();

		std::function<void(HWND hwnd, MainThreadDispatcher& main_thread_dispatcher)> on_init;
		std::function<void()> on_update;
		std::function<void(int id)> on_hotkey_pressed;
		std::function<void()> on_shutdown;

	private:
		// User defined app data
		// DarksApp& app_;
		std::unique_ptr<MainThreadDispatcher> dispatcher_;

		// Window data
		WNDCLASSEXW wc_;
		HWND hwnd_;
		UINT width_ = 2560;
		UINT height_ = 1440;
		LPCWSTR name_ = L"Dark Arks External Overlay";

		// D3D11 data
		ID3D11Device* device_ = nullptr;
		ID3D11DeviceContext* device_context_ = nullptr;
		IDXGISwapChain* swap_chain_ = nullptr;
		UINT resize_width_ = 0, resize_height_ = 0;
		ID3D11RenderTargetView* render_target_view_ = nullptr;

		bool CreateDeviceD3D(HWND hwnd);
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();

		// Handle WndProc messages
		LRESULT HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param);

		static LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
		{
			if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param))
				return true;

			OverlayWindowBase* window_base = NULL;

			if (msg == WM_CREATE) {
				// Set user data to be retrieved on subsquent calls
				CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(l_param);
				window_base = reinterpret_cast<OverlayWindowBase*>(pCreate->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window_base);
				return 0;
			}
			else
				window_base = (OverlayWindowBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			if (window_base)
				return window_base->HandleMessage(msg, w_param, l_param);
			else
				return DefWindowProc(hwnd, msg, w_param, l_param);
		}
	};
}

#endif