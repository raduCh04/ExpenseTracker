#include <iostream>
#include <vector>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "Application.h"

static constexpr int default_width = 800;
static constexpr int default_height = 600;

static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Application::Application() 
	: m_Window(nullptr), m_Width(default_width), m_Height(default_height), m_IsRunning(false) {
	// Set GLFW callbacks
	glfwSetErrorCallback(error_callback);

	// Init GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to init glfw!\n";
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	m_Window = glfwCreateWindow(m_Width, m_Height, "Am I Poor ?", nullptr, nullptr);
	if (m_Window == nullptr) {
		std::cerr << "Failed to create window!\n";
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(m_Window);
	glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
	glfwSwapInterval(1);
	// Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

Application::~Application() {
	//Shutdown ImGui
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	//Shutdown GLFW
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}
