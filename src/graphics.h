#pragma once
#include "window.h"
#include "vector2.h"

struct Graphics
{
	friend Window;

	static void Quad(Vector2 pos, Vector2 size = Vector2::one, const Color& color = Color::white);
	static void Circle(Vector2 pos, float radius, const Color& color = Color::white);

private:
	static void Init(unsigned int width, unsigned int height);
	static void Shutdown();
};
