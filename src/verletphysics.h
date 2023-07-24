#pragma once
#include "verletobj.h"
#include "vector2.h"
#include <vector>
#include <iostream>

struct VerletPhysics
{
	float gravity = 0.0f;
	Vector2 center = Vector2::zero;
	float radius = 0.0f;
	unsigned int substeps = 8;
	std::vector<VerletObj> objects = {};

	void Update(float dt);
	void Gravity();
	void Constraint();
	void Collisions();
	void Move(float dt);
};
