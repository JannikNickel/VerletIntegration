#pragma once
#include "ecs/component.h"
#include "structs/matrix4.h"

struct Transform : Component<Transform>
{
	Matrix4 value;

	Transform(Matrix4 value) : value(value) { }

	Vector2& Position()
	{
		return *reinterpret_cast<Vector2*>(&value.cells[12]);
	}
};
