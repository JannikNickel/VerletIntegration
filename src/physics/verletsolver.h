#pragma once
#include "constraint.h"
#include "verletobj.h"
#include "ecs/world.h"
#include <vector>

enum class SolverUpdateMode
{
	/// <summary>
	/// Solver updates once per frame rendered using the frame delta time step
	/// </summary>
	FrameDeltaTime,
	/// <summary>
	/// Solver updates once per frame rendered using a fixed time step
	/// </summary>
	FrameFixedStep,
	/// <summary>
	/// Solver updates at a fixed frame rate (0 or multiple times each rendered frame) using a fixed delta time step
	/// </summary>
	FixedFrameRate
};

class VerletSolver
{
public:
	float gravity;
	unsigned int substeps;
	bool collision;
	SolverUpdateMode updateMode;

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
