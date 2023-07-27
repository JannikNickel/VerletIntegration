#pragma once
#include "structs/vector2.h"
#include "structs/color.h"
#include "structs/matrix4.h"

struct VerletObj
{
	unsigned int id;
	Vector2 pos;
	Vector2 prevPos;
	Vector2 acc;
	float mass;
	float radius;
	Color color;
	Matrix4 matrix;

	VerletObj(unsigned int id, Vector2 pos, float mass, float radius, Color color) : id(id), pos(pos), prevPos(pos), acc(Vector2::zero), mass(mass), radius(radius), color(color)
	{
		matrix = Matrix4::PositionScale2d(pos, radius * 2.0f);
	}
};
