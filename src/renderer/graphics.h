#pragma once
#include "engine/window.h"
#include "structs/vector2.h"
#include "structs/matrix4.h"

struct Graphics
{
	friend Window;

	static const unsigned int instancingLimit = 1024;

	static void SetClearColor(Color color);
	static void Quad(Vector2 pos, Vector2 size = Vector2::one, const Color& color = Color::white);
	static void Circle(Vector2 pos, float radius, const Color& color = Color::white);
	static void QuadsInstanced(const Vector2* positions, const Vector2* sizes, const Color* colors, int instanceCount);
	static void CirclesInstanced(const Vector2* positions, const float* radii, const Color* colors, int instanceCount);
	static void CirclesInstanced(const Matrix4* matrices, const Color* colors, int instanceCount);

private:
	static void Init(Window* window, unsigned int width, unsigned int height);
	static void Shutdown();
};
