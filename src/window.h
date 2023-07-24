#pragma once
#include "color.h"
#include <functional>

class Window
{
public:
	Color clearColor = Color::From32(30, 30, 30);

	Window(unsigned int width, unsigned int height, const char* title, int x = -1, int y = -1);

	void Show(std::function<void(double)> update);
	void Close();
	
	float LastFrameRenderTime();

private:
	unsigned int width;
	unsigned int height;
	const char* title;
	int x;
	int y;

	float renderTime;

	struct GLFWwindow* window;
};
