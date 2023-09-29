#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Defs.h"

#include "GLFW/glfw3.h"

class Application
{
public:
	Application();
	Application(const Application &) = delete;
	Application(Application &&) = delete;
	virtual ~Application();

	virtual void Run() = 0;

	inline i32 GetWidth() const { return m_Width; }
	inline i32 GetHeight() const { return m_Height; }
	inline void SetWidth(i32 width) { m_Width = width; }
	inline void SetHeight(i32 height) { m_Height = height; }
	inline GLFWwindow *GetWindow() const { return m_Window; }

	inline void Stop() { m_IsRunning = false; }
	inline bool IsRunning() const { return m_IsRunning == glfwWindowShouldClose(m_Window); }

private:
	void InitGLFW();
	void InitImGui();

private:
	GLFWwindow *m_Window;
	i32 m_Width, m_Height;
	bool m_IsRunning;
};