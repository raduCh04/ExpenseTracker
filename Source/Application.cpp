#include <iostream>

#include "Application.h"

static constexpr int default_width = 800;
static constexpr int default_height = 600;

LRESULT Application::WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	LRESULT result = 0;

	Application *this_pointer = nullptr;
	switch (message) {
		case WM_CREATE: {
			CREATESTRUCTA *window_struct = reinterpret_cast<CREATESTRUCTA*>(lparam);
			this_pointer = reinterpret_cast<Application *>(window_struct->lpCreateParams);
			if (this_pointer == nullptr) {
				MessageBoxA(NULL, "Failed to create pointer beaci!", "Error", MB_OK | MB_ICONERROR);
				exit(EXIT_FAILURE);
			}
			SetWindowLongPtrA(window, GWLP_USERDATA, (LONG_PTR)this_pointer);	
		} break;

		case WM_DESTROY:
		case WM_QUIT: {
			this_pointer = reinterpret_cast<Application*>(GetWindowLongPtrA(window, GWLP_USERDATA));
			this_pointer->Stop();
			PostQuitMessage(EXIT_SUCCESS);
		} break;

		case WM_PAINT: {
			this_pointer = reinterpret_cast<Application*>(GetWindowLongPtrA(window, GWLP_USERDATA));
			RECT client_rect;
			RECT text;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(window, &ps);

			GetClientRect(window, &client_rect);
			
			int width = client_rect.right - client_rect.left;
			int height = client_rect.bottom - client_rect.top;

			HRGN bg_region = CreateRectRgnIndirect(&client_rect);
			HBRUSH brush = CreateSolidBrush(RGB(200, 200, 200));
			FillRgn(hdc, bg_region, brush);

			HPEN pen = CreatePen(PS_DASH, 1, (RGB(0, 255, 0)));
			SelectObject(hdc, pen);
			SetBkColor(hdc, RGB(1, 1, 1));
			Rectangle(hdc, (width / 2 - 100), height / 2 - 100, width / 2 + 100, height / 2 + 100);

			SetBkColor(hdc, RGB(0xFF, 0xFF, 0xFF));
			SetRect(&text, width / 2 - 100, height / 2, width / 2 + 100, height / 2);
			DrawTextA(hdc, (LPSTR)TEXT("Hello friends"), -1, &text, DT_CENTER | DT_NOCLIP);

			EndPaint(window, &ps);
		} break;
		
		default: {
			result = DefWindowProcA(window, message, wparam, lparam);
		} break;
	}

	return result;
}

Application::Application() 
	: m_WindowClass({}), m_WindowHandle(nullptr), m_Width(default_width), m_Height(default_height), m_IsRunning(false) {
		m_WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		m_WindowClass.hCursor = LoadCursorA(nullptr, (LPSTR)IDC_ARROW);
		m_WindowClass.lpfnWndProc = WndProc;
		m_WindowClass.lpszClassName = "Main Window Class";

		if (RegisterClassA(&m_WindowClass) == INVALID_ATOM) {
			MessageBoxA(NULL, "Failed to create a window!", "ERROR", MB_OK | MB_ICONERROR);
			exit(EXIT_FAILURE);
		}

		m_WindowHandle = CreateWindowExA(0, m_WindowClass.lpszClassName, 
						"Am I Poor?", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						CW_USEDEFAULT, CW_USEDEFAULT,
						m_Width, m_Height, NULL, NULL, NULL, this);
		if (m_WindowHandle == nullptr) {
			MessageBoxA(NULL, "Failed to create a window!", "ERROR", MB_OK | MB_ICONERROR);
			exit(EXIT_FAILURE);
		}

		m_IsRunning = true;
}