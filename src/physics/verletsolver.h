#pragma once
#include "constraint.h"
#include "verletobj.h"
#include "ecs/world.h"
#include <vector>

class VerletSolver
{
public:
	float gravity;
	unsigned int substeps;
	bool collision;

	VerletSolver(EcsWorld& ecs, IConstraint* constraint, float timeStep, float gravity, unsigned int substeps = 1);
	void Update(float dt);

private:
	float timeStep;
	EcsWorld& ecs;
	IConstraint* constraint;

	void Simulate(float dt);
	void Gravity();
	void Constraint();
	void Collisions();
	void Move(float dt);
};
