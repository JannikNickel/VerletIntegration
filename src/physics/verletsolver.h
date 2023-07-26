#pragma once
#include "constraint.h"
#include "verletobj.h"
#include <vector>

class VerletSolver
{
public:
	float gravity;
	unsigned int substeps;
	std::vector<VerletObj> objects = {};

	VerletSolver(float timeStep, IConstraint* constraint, float gravity, unsigned int substeps = 1);
	void Update(float dt);

private:
	float timeStep;
	IConstraint* constraint;

	void Simulate(float dt);
	void Gravity();
	void Constraint();
	void Collisions();
	void Move(float dt);
};
