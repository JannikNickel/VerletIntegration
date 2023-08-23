#pragma once
#include "ecs/component.h"
#include "structs/vector2.h"

struct Particle : Component<Particle>
{
	float radius;
	float mass;
	float bounciness;
	Vector2 prevPos;
	Vector2 acc;

	Particle(float radius, float mass, float bounciness, Vector2 prevPos, Vector2 acc) : radius(radius), mass(mass), bounciness(bounciness), prevPos(prevPos), acc(acc) { }
};
