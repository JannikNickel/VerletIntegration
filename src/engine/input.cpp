#include "input.h"
#include "GLFW/glfw3.h"
#include <utility>

static double mouseX = 0.0f;
static double mouseY = 0.0f;
static double mouseDeltaX = 0.0f;
static double mouseDeltaY = 0.0f;
static std::pair<unsigned char, bool> keyStates[static_cast<int>(KeyCode::_END)] = { std::pair<unsigned char, bool>(0, false) };

static void HandleGLFWKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
static void HandleGLFWInput(GLFWwindow* window, int key, int action, int mods);

Vector2 Input::MousePosition()
{
	return Vector2(mouseX, mouseY);
}

Vector2 Input::MouseDelta()
{
	return Vector2(mouseDeltaX, mouseDeltaY);
}

static bool CompareKeyState(KeyCode code, int target)
{
	int index = static_cast<int>(code);
	if(index < 0 || index >= static_cast<int>(KeyCode::_END))
	{
		return false;
	}
	return keyStates[index].first == target;
}

bool Input::KeyPressed(KeyCode code)
{
	return CompareKeyState(code, 1);
}

bool Input::KeyHeld(KeyCode code)
{
	return CompareKeyState(code, 2);
}

bool Input::KeyReleased(KeyCode code)
{
	return CompareKeyState(code, 3);
}

void Input::Init(GLFWwindow* window)
{
	if(glfwRawMouseMotionSupported())
	{
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	glfwSetKeyCallback(window, HandleGLFWKeyInput);
	glfwSetMouseButtonCallback(window, HandleGLFWInput);
}

void Input::Update(GLFWwindow* window)
{
	double px = mouseX;
	double py = mouseY;
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	glfwGetCursorPos(window, &mouseX, &mouseY);
	mouseY = h - mouseY;
	mouseDeltaX = mouseX - px;
	mouseDeltaY = mouseY - py;

	for(int i = 0; i < static_cast<int>(KeyCode::_END); i++)
	{
		if(!keyStates[i].second)
		{
			if(keyStates[i].first == 1)
			{
				keyStates[i].first = 2;
			}
			else if(keyStates[i].first == 3)
			{
				keyStates[i].first = 0;
			}
		}
		keyStates[i].second = false;
	}
}

void HandleGLFWKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	HandleGLFWInput(window, key, action, mods);
}

void HandleGLFWInput(GLFWwindow* window, int key, int action, int mods)
{
	if(key >= static_cast<int>(KeyCode::_END))
	{
		return;
	}
	if(action == GLFW_PRESS || action == GLFW_RELEASE)
	{
		keyStates[key].first = action == GLFW_PRESS ? 1 : 3;
		keyStates[key].second = true;
	}
}
