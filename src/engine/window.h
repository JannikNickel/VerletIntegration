#pragma once
#include "structs/color.h"
#include "structs/vector2.h"
#include <functional>

class Window
{
public:
	Color clearColor = Color::From32(30, 30, 30);

	Window(unsigned int width, unsigned int height, const char* title, int x = -1, int y = -1, bool vSync = true);

	void Show(std::function<void(double)> update);
	void Close();
	Vector2 Size();
	float LastFrameRenderTime();

private:
	unsigned int width;
	unsigned int height;
	const char* title;
	int x;
	int y;
	bool vSync;

	float renderTime;

	struct GLFWwindow* window;
};
