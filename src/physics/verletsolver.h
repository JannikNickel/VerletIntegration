#pragma once
#include "constraint.h"
#include "partitioning.h"
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

	VerletSolver(EcsWorld& ecs, IConstraint* constraint, float timeStep, float gravity, unsigned int substeps = 1, float partitioningSize = 25.0f);
	void Update(float dt);

private:
	float timeStep;
	float partitioningSize;
	EcsWorld& ecs;
	IConstraint* constraint;
	PartitioningGrid partitioning;

	void Simulate(float dt);
	void Gravity();
	void Constraint();
	void Collisions();
	void SolveCell(PartitioningCell& cell);
	void SolveCells(PartitioningCell& cell0, PartitioningCell& cell1);
	void Solve(Transform& aTransform, PhysicsCircle& a, Transform& bTransform, PhysicsCircle& b);
	void Move(float dt);
};
