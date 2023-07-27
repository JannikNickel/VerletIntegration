#pragma once
#include "engine/window.h"
#include "structs/vector2.h"
#include "structs/matrix4.h"

struct Graphics
{
	friend Window;

	const static unsigned int instancingLimit = 1024;

	static void SetClearColor(Color color);
	static void Quad(Vector2 pos, Vector2 size = Vector2::one, const Color& color = Color::white);
	static void Circle(Vector2 pos, float radius, const Color& color = Color::white);
	static void QuadsInstanced(Vector2* positions, Vector2* sizes, Color* colors, int instanceCount);
	static void CirclesInstanced(Vector2* positions, float* radii, Color* colors, int instanceCount);
	static void CirclesInstanced(Matrix4* matrices, Color* colors, int instanceCount);

private:
	static void Init(Window* window, unsigned int width, unsigned int height);
	static void Shutdown();
};
