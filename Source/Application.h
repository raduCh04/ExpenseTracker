#pragma once


#include <string>
#include <vector>
#include <iostream>

#include "GLFW/glfw3.h"

class Application {
	public:
		Application();
		~Application();

		virtual void Run() = 0;

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline GLFWwindow *GetWindow() const { return m_Window; }

		inline void Stop() { m_IsRunning = false; }
        inline bool IsRunning() const { return m_IsRunning == glfwWindowShouldClose(m_Window); }
	private:
		GLFWwindow *m_Window;
		int m_Width, m_Height;
		bool m_IsRunning;
};