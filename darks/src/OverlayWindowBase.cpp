#include "OverlayWindowBase.h"

namespace Darks {
	OverlayWindowBase::OverlayWindowBase(DarkArksApp& app) :
		app_(app)
	{ }

	/// <summary>
	/// Runs the app, including initialization, the message loop, and cleanup.
	/// </summary>
	void OverlayWindowBase::Start() {
		WNDCLASSEXW wc = {
			sizeof(wc),
			CS_CLASSDC,
			OverlayWindowBase::WndProc,
			0L,
			0L,
			GetModuleHandle(nullptr),
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			L"Dark Arks External Overlay Class",
			nullptr
		};

		// Register class
		if (!RegisterClassExW(&wc)) {
			throw std::runtime_error("Failed to register the window class.");
		}

		hwnd_ = ::CreateWindowExW(
			WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST,
			wc.lpszClassName,
			name_,
			WS_POPUP,
			0, 0, width_, height_,
			nullptr,
			nullptr,
			wc.hInstance,
			// Provide the pointer to our app data
			this);

		dispatcher_ = std::make_unique<MainThreadDispatcher>(hwnd_);

		// Set layered window attributes so it will be rendered transparently using an alpha channel
		if (!SetLayeredWindowAttributes(hwnd_, RGB(0, 0, 0), BYTE(255), LWA_ALPHA)) {
			UnregisterClassW(wc.lpszClassName, wc.hInstance);
			throw std::runtime_error("Failed to set layered window attributes.");
		}

		{ // Extends the client area of the window to take up the entire window including the OS surrounding area.
			RECT client_rect{};
			if (!GetClientRect(hwnd_, &client_rect)) {
				UnregisterClassW(wc.lpszClassName, wc.hInstance);
				throw std::runtime_error("Failed to get client rect for extending frame into client area.");
			}

			RECT window_rect{};
			if (!GetWindowRect(hwnd_, &window_rect)) {
				UnregisterClassW(wc.lpszClassName, wc.hInstance);
				throw std::runtime_error("Failed to get the window rect for extending the frame into the client area.");
			}

			POINT diff{};
			if (!ClientToScreen(hwnd_, &diff)) {
				UnregisterClassW(wc.lpszClassName, wc.hInstance);
				throw std::runtime_error("Failed to get client to screen diff for extending the frame into the client area.");
			}

			const MARGINS margins{
				window_rect.left + (diff.x - window_rect.left),
				window_rect.top + (diff.y - window_rect.top),
				client_rect.right,
				client_rect.bottom
			};

			if (!SUCCEEDED(DwmExtendFrameIntoClientArea(hwnd_, &margins))) {
				UnregisterClassW(wc.lpszClassName, wc.hInstance);
				throw std::runtime_error("Failed to extend frame into client area.");
			}
		}

		// Initialize Direct3D
		if (!CreateDeviceD3D(hwnd_))
		{
			CleanupDeviceD3D();
			UnregisterClassW(wc.lpszClassName, wc.hInstance);
			throw std::runtime_error("Failed to create D3D11 device_.");
		}

		DWORD r = GetLastError();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		if (!ImGui_ImplWin32_Init(hwnd_)) {
			CleanupDeviceD3D();
			UnregisterClassW(wc.lpszClassName, wc.hInstance);
			throw std::runtime_error("Failed to initialize the imgui win32 implemenation.");
		}
		if (!ImGui_ImplDX11_Init(device_, device_context_)) {
			CleanupDeviceD3D();
			UnregisterClassW(wc.lpszClassName, wc.hInstance);
			throw std::runtime_error("Failed to initialize the imgui directx 11 implement");
		}

		// Load fonts
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		ImFont* hud_font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 40.0f);

		// Call the app's init function
		app_.Init(hwnd_, *dispatcher_, hud_font);

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);

		// Show the window
		if (ShowWindow(hwnd_, SW_SHOWDEFAULT)) {
			CleanupDeviceD3D();
			UnregisterClassW(wc.lpszClassName, wc.hInstance);
			throw std::runtime_error("Failed to show the window.");
		}
		if (!UpdateWindow(hwnd_)) {
			CleanupDeviceD3D();
			UnregisterClassW(wc.lpszClassName, wc.hInstance);
			throw std::runtime_error("Failed to update the window.");
		}

		// Main loop
		bool done = false;
		while (!done)
		{
			// Poll and handle messages (inputs, window resize, etc.)
			// See the WndProc() function below for our to dispatch events to the Win32 backend.
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
			{
				if (msg.message == WM_HOTKEY) {
					// Propogate hotkey pressed
					app_.OnHotKeyPressed(msg.wParam);
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					done = true;
			}
			if (done)
				break;

			// Handle window resize (we don't resize directly in the WM_SIZE handler)
			if (resize_width_ != 0 && resize_height_ != 0)
			{
				CleanupRenderTarget();
				swap_chain_->ResizeBuffers(0, resize_width_, resize_height_, DXGI_FORMAT_UNKNOWN, 0);
				resize_width_ = resize_height_ = 0;
				CreateRenderTarget();
			}

			// Start the Dear ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();


			/*ImGui::GetBackgroundDrawList()->AddRectFilled(
				{ static_cast<float>(width_) / 2.f - 10.5f, static_cast<float>(height_) / 2.f - 2.f },
				{ static_cast<float>(width_) / 2.f + 10.5f, static_cast<float>(height_) / 2.f + 2.f },
				ImColor(2.f, 0.f, 0.f),
				0.5f
			);

			ImGui::GetBackgroundDrawList()->AddRectFilled(
				{ static_cast<float>(width_) / 2.f - 2.f, static_cast<float>(height_) / 2.f - 10.5f },
				{ static_cast<float>(width_) / 2.f + 2.f, static_cast<float>(height_) / 2.f + 10.5f },
				ImColor(2.f, 0.f, 0.f),
				0.5f
			);*/

			app_.OnUpdate();


			// Rendering
			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			device_context_->OMSetRenderTargets(1, &render_target_view_, nullptr);
			device_context_->ClearRenderTargetView(render_target_view_, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			swap_chain_->Present(1, 0); // Present with vsync
			//g_pSwapChain->Present(0, 0); // Present without vsync
		}

		app_.OnShutdown();

		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		DestroyWindow(hwnd_);
		UnregisterClassW(wc.lpszClassName, wc.hInstance);
	}

	bool OverlayWindowBase::CreateDeviceD3D(HWND hWnd) {
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		HRESULT res = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			featureLevelArray,
			2,
			D3D11_SDK_VERSION,
			&sd,
			&swap_chain_,
			&device_,
			&featureLevel,
			&device_context_);
		if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
			res = D3D11CreateDeviceAndSwapChain(
				nullptr,
				D3D_DRIVER_TYPE_WARP,
				nullptr,
				createDeviceFlags,
				featureLevelArray,
				2,
				D3D11_SDK_VERSION,
				&sd,
				&swap_chain_,
				&device_,
				&featureLevel,
				&device_context_);
		if (res != S_OK)
			return false;

		CreateRenderTarget();
		return true;
	}

	void OverlayWindowBase::CleanupDeviceD3D() {
		CleanupRenderTarget();
		if (swap_chain_) { swap_chain_->Release(); swap_chain_ = nullptr; }
		if (device_context_) { device_context_->Release(); device_context_ = nullptr; }
		if (device_) { device_->Release(); device_ = nullptr; }
	}

	void OverlayWindowBase::CreateRenderTarget() {
		ID3D11Texture2D* pBackBuffer;
		swap_chain_->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		device_->CreateRenderTargetView(pBackBuffer, nullptr, &render_target_view_);
		pBackBuffer->Release();
	}

	void OverlayWindowBase::CleanupRenderTarget() {
		if (render_target_view_) { render_target_view_->Release(); render_target_view_ = nullptr; }
	}

	LRESULT OverlayWindowBase::HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param)
	{
		switch (msg) {
		case WM_SIZE:
			if (w_param == SIZE_MINIMIZED)
				return 0;
			resize_width_ = (UINT)LOWORD(l_param); // Queue resize
			resize_height_ = (UINT)HIWORD(l_param);
			return 0;
		case WM_SYSCOMMAND:
			if ((w_param & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		// Handle custom messages
		if (msg >= WM_USER && msg < WM_APP && dispatcher_) {
			return dispatcher_->Handle(msg);
		}

		return DefWindowProc(hwnd_, msg, w_param, l_param);
	}
}