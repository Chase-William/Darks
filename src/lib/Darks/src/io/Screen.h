#ifndef DARKS_IO_SCREEN_H_
#define DARKS_IO_SCREEN_H_

#include "../Framework.h"

#include <vector>
#include <atlimage.h>
#include <stdexcept>
#include <optional>

#include "../Log.h"
#include "Pixel.h"

namespace Darks::IO::Screen {
	const float INCH_TO_METER_RATIO = 39.3701f;

	/// <summary>
	/// Gets the screen's pixel color at the specified x and y.
	/// </summary>
	/// <returns>The color at the specified coordinates.</returns>
	static Color GetPixelColor(Point pos) {
		HDC screen_device_ctx = GetDC(NULL);
		// Failed to get dc
		if (!screen_device_ctx) {
			DARKS_ERROR("Was unable to get the screen device context.");
			throw std::runtime_error("Was unable to get the screen device context to inspect pixel color.");
		}
		
		COLORREF color_ref = GetPixel(screen_device_ctx, pos.x, pos.y);
		
		// Failed to get pixel color
		if (color_ref == CLR_INVALID) {
			DARKS_ERROR("The result from win32 sys call GetPixel returned the invalid flag CLR_INVALID.");
			throw std::runtime_error("Color returned from win32 GetPixel sys call was invalid (CLR_INVALID).");
		}
		
		if (!ReleaseDC(NULL, screen_device_ctx)) {
			DARKS_ERROR("Was unable to release the screen device context.");
		}
		
		Color color{};
		color.red = GetRValue(color_ref);
		color.green = GetGValue(color_ref);
		color.blue = GetBValue(color_ref);
		
		return color;
	}

	struct Screenshot {
		Screenshot(
			const char* buffer,
			const int size
		) :
			buffer(buffer),
			size(size)
		{ }

		const char* buffer;
		const int size;
	};

	/// <summary>
	/// Takes a screenshot of the primary monitor using the provided dimensions if they exists, otherwise the entire screen is captured.
	/// </summary>
	/// <param name="dimensions">Optional dimensions to be used when capturing a screenshot from the primary monitor.</param>
	static std::unique_ptr<std::vector<char>> GetScreenshot(std::optional<Rect> dimensions = std::nullopt) {
		HDC screen_ctx = GetDC(NULL);		
		HDC dc_target = CreateCompatibleDC(screen_ctx);
		if (!dc_target) {
			auto msg = "Failed to get a compatible handle to the screen device context.";
			DARKS_ERROR(msg);
			ReleaseDC(NULL, screen_ctx);
			throw std::runtime_error(msg);
		}

		HBITMAP h_bmp_target;
		HGDIOBJ bmp_obj;

		// If specific dimensions are provided, use them for screenshot
		if (dimensions.has_value()) {
			Rect dims_value = dimensions.value();
			h_bmp_target = CreateCompatibleBitmap(screen_ctx, dims_value.Width(), dims_value.Height());
			bmp_obj = SelectObject(dc_target, h_bmp_target);
			BitBlt(dc_target, 0, 0, dims_value.Width(), dims_value.Height(), screen_ctx, dims_value.left, dims_value.top, SRCCOPY);
		}
		else {
			// get screen dimensions
			int primary_monitor_width = GetSystemMetrics(SM_CXSCREEN);
			int primary_monitor_height = GetSystemMetrics(SM_CYSCREEN);

			h_bmp_target = CreateCompatibleBitmap(screen_ctx, primary_monitor_width, primary_monitor_height);
			bmp_obj = SelectObject(dc_target, h_bmp_target);
			BitBlt(dc_target, 0, 0, primary_monitor_width, primary_monitor_height, screen_ctx, 0, 0, SRCCOPY);
		}		
		
		// save bitmap to clipboard
		/*OpenClipboard(NULL);
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, h_bmp_target);
		CloseClipboard();*/
		/*BITMAP bmp{};
		GetObject(h_bmp_target, sizeof(BITMAP), &bmp);

		BITMAPFILEHEADER   bmp_file_header{};
		BITMAPINFOHEADER   bmp_info_header{};

		bmp_info_header.biSize = sizeof(BITMAPINFOHEADER);
		bmp_info_header.biWidth = bmp.bmWidth;
		bmp_info_header.biHeight = bmp.bmHeight;
		bmp_info_header.biPlanes = 1;
		bmp_info_header.biBitCount = 32;
		bmp_info_header.biCompression = BI_RGB;

		bmp_info_header.biXPelsPerMeter = GetDeviceCaps(screen_ctx, LOGPIXELSX) * INCH_TO_METER_RATIO;
		bmp_info_header.biYPelsPerMeter = GetDeviceCaps(screen_ctx, LOGPIXELSY) * INCH_TO_METER_RATIO;*/
		
		// Get DPI of screen vertical / horizontal
		/*HDC screen = GetDC(NULL);
		dpiX = (GetDeviceCaps(screen, LOGPIXELSX));
		dpiY = (GetDeviceCaps(screen, LOGPIXELSY));
		ReleaseDC(NULL, screen);*/
				
		// aligning bits for words?
		//DWORD bmp_content_size = ((bmp.bmWidth * bmp_info_header.biBitCount + 31) / 32) * 4 * bmp.bmHeight;
		//DWORD bmp_header_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		//DWORD bmp_full_size = bmp_content_size + bmp_header_size;
		//
		//// Allocate bitmap buffer
		//char* bmp_buf = (char*)malloc(bmp_full_size);
		//if (!bmp_buf) {

		//}

		//// The offset, in bytes, from the beginning of the BITMAPFILEHEADER structure to the bitmap bits
		//bmp_file_header.bfOffBits = bmp_header_size;
		//// Create pointer to the beginning of bmp content
		//char* bmp_content_buf = bmp_buf + bmp_header_size;

		//GetDIBits(
		//	dc_target,
		//	h_bmp_target,
		//	0,
		//	(UINT)bmp.bmHeight,
		//	bmp_content_buf,
		//	(BITMAPINFO*)&bmp_info_header,
		//	DIB_RGB_COLORS
		//);

		//// Size of the file.
		//bmp_file_header.bfSize = bmp_full_size;
		//// bfType must always be BM for Bitmaps.
		//bmp_file_header.bfType = 0x4D42; // BM.
		//// Finish filling out buffer of entire bmpBuffer
		//std::memcpy(bmp_buf, &bmp_file_header, sizeof(bmp_file_header));
		//std::memcpy(bmp_buf + sizeof(bmp_file_header), &bmp_info_header, sizeof(bmp_info_header));
		//
		//HANDLE hFile = CreateFile(L"test.bmp",
		//	GENERIC_WRITE,
		//	0,
		//	NULL,
		//	CREATE_ALWAYS,
		//	FILE_ATTRIBUTE_NORMAL, NULL);
		//DWORD written_bytes = 0;
		//WriteFile(hFile, (LPSTR)&bmp_file_header, sizeof(BITMAPFILEHEADER), &written_bytes, NULL);
		//WriteFile(hFile, (LPSTR)&bmp_info_header, sizeof(BITMAPINFOHEADER), &written_bytes, NULL);
		//WriteFile(hFile, (LPSTR)bmp_content_buf, bmp_content_size, &written_bytes, NULL);

		std::unique_ptr<std::vector<char>> buf = std::make_unique<std::vector<char>>();
		IStream* stream = NULL;
		HRESULT hr = CreateStreamOnHGlobal(0, TRUE, &stream);
		CImage image{};
		ULARGE_INTEGER stream_size;

		image.Attach(h_bmp_target);
		image.Save(stream, Gdiplus::ImageFormatJPEG);
		IStream_Size(stream, &stream_size);
		DWORD len = stream_size.LowPart;
		IStream_Reset(stream);
		buf->resize(len);
		IStream_Read(stream, &(*buf)[0], len);
		stream->Release();
		
		// just testing if the buf contains the correct data
		/*std::fstream fi;
		fi.open("aaashot.jpg", std::fstream::binary | std::fstream::out);
		fi.write(reinterpret_cast<const char*>(&(*buf)[0]), buf->size() * sizeof(BYTE));
		fi.close();*/

		// clean up
		// CloseHandle(hFile);
		SelectObject(dc_target, bmp_obj);
		DeleteDC(dc_target);
		ReleaseDC(NULL, screen_ctx);
		DeleteObject(h_bmp_target);
		return buf;
	}
}

#endif