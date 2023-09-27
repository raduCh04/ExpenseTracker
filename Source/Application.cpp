#include <iostream>
#include <vector>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "Application.h"

static void error_callback(i32 error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void framebuffer_size_callback(GLFWwindow *window, i32 width, i32 height)
{
	auto p = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
	if (p != nullptr)
	{
		p->SetWidth(width);
		p->SetHeight(height);
	}
	else
	{
		std::cerr << "Failed to get user pointer\n";
	}
	glViewport(0, 0, width, height);
}

Application::Application()
	: m_Window(nullptr), m_Width(0), m_Height(0), m_IsRunning(false)
{
	InitGLFW();
	InitImGui();
}

Application::~Application()
{
	// Shutdown ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Shutdown GLFW
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Application::InitGLFW()
{
	glfwSetErrorCallback(error_callback);

	// Init GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to init glfw!\n";
		exit(EXIT_FAILURE);
	}
	i32 count = 0;
	auto m_Monitors = glfwGetMonitors(&count);
	if (m_Monitors == nullptr)
	{
		std::cerr << "Failed to get current monitors!\n";
		exit(EXIT_FAILURE);
	}
	const auto mode = glfwGetVideoMode(m_Monitors[0]);
	if (mode == nullptr)
	{
		std::cerr << "Failed to get video mode!\n";
	}
	else
	{
		m_Width  = mode->width / 1.5;
		m_Height = mode->height / 16 * 9;
	}

	i32 monitorX = 0, monitorY = 0;
	glfwGetMonitorPos(m_Monitors[0], &monitorX, &monitorY);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	m_Window = glfwCreateWindow(m_Width, m_Height, "Am I Poor ?", nullptr, nullptr);
	if (m_Window == nullptr)
	{
		std::cerr << "Failed to create window!\n";
		exit(EXIT_FAILURE);
	}
	glfwSetWindowUserPointer(m_Window, this);
	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1);
	glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, GLFW_FALSE);
	glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

	glfwDefaultWindowHints();

	glfwSetWindowPos(m_Window,
					 monitorX + (mode->width - m_Width) / 2,
					 monitorY + (mode->height - m_Height) / 2);
	glfwShowWindow(m_Window);

	glfwGetWindowSize(m_Window, &m_Width, &m_Height);
}

void Application::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_None;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}
