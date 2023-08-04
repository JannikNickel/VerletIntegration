#pragma once
#include "ecs/component.h"
#include "structs/vector2.h"

struct PhysicsCircle : Component<PhysicsCircle>
{
	float radius;
	float mass;
	Vector2 prevPos;
	Vector2 acc;

	PhysicsCircle(float radius, float mass, Vector2 prevPos, Vector2 acc) : radius(radius), mass(mass), prevPos(prevPos), acc(acc) { }
};
