#include "window.h"
#include "input.h"
#include "graphics.h"

#include <iostream>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef std::chrono::high_resolution_clock Clock;
static void HandleGLFWError(int error, const char* description);

Window::Window(unsigned int width, unsigned int height, const char* title, int x, int y) : width(width), height(height), title(title), x(x), y(y), window(nullptr)
{

}

void Window::Show(std::function<void(double)> update)
{
	if(!glfwInit())
	{
		std::cerr << "glfwInit failed!" << std::endl;
		return;
	}

	glfwSetErrorCallback(HandleGLFWError);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if(window == nullptr)
	{
		std::cerr << "GLFW window creation failed!" << std::endl;
		glfwTerminate();
		return;
	}

	int xPos, yPos;
	glfwGetWindowPos(window, &xPos, &yPos);
	glfwSetWindowPos(window, x != -1 ? x : xPos, y != -1 ? y : yPos);

	glfwMakeContextCurrent(window);
	//glfwSwapInterval(0);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to load GL!" << std::endl;
		glfwTerminate();
		return;
	}

	glViewport(0, 0, width, height);

	Input::Init(window);
	Graphics::Init(width, height);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	auto tPrev = Clock::now();
	while(!glfwWindowShouldClose(window))
	{
		auto tCurr = Clock::now();
		double dt = std::chrono::duration<double>(tCurr - tPrev).count();
		tPrev = tCurr;

		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		Input::Update(window);
		update(dt);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	Graphics::Shutdown();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::Close()
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void HandleGLFWError(int error, const char* description)
{
	std::cerr << "GLFW ERROR (" << description << ")" << std::endl;
}
