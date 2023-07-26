#pragma once
#include "structs/vector2.h"
#include "structs/color.h"

struct VerletObj
{
	unsigned int id;
	Vector2 pos;
	Vector2 prevPos;
	Vector2 acc;
	float mass;
	float radius;
	Color color;

	VerletObj(unsigned int id, Vector2 pos, float mass, float radius, Color color) : id(id), pos(pos), prevPos(pos), acc(Vector2::zero), mass(mass), radius(radius), color(color)
	{
		
	}
};
