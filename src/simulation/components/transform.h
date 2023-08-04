#pragma once
#include "ecs/component.h"
#include "structs/matrix4.h"

struct Transform : Component<Transform>
{
	Matrix4 value;

	Vector2& Position()
	{
		return *reinterpret_cast<Vector2*>(&value.cells[12]);
	}

	Transform(Matrix4 value) : value(value) { }
};
