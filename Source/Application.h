#pragma once

#include <Windows.h>
#include <iostream>

class Application {
	public:
		Application();

		virtual void Run() = 0;

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }

		inline void Stop() { m_IsRunning = false; }
        inline bool IsRunning() const { return m_IsRunning; }
	private:
		static LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	protected:
		WNDCLASSA m_WindowClass;
		HWND m_WindowHandle;
	private:
		int m_Width, m_Height;
		bool m_IsRunning;
};