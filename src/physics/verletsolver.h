#pragma once
#include "solversettings.h"
#include "constraint.h"
#include "partitioning.h"
#include "ecs/world.h"
#include "utils/framecounter.h"
#include "structs/vector2.h"
#include <cstdint>

class VerletSolver
{
public:
	Vector2 gravity;
	uint32_t substeps;
	bool collision;
	SolverUpdateMode updateMode;

	VerletSolver(EcsWorld& ecs, IConstraint& constraint, const SolverSettings& settings);
	void Update(float dt);
	const FrameCounter& BroadPhaseCounter() const;
	const FrameCounter& NarrowPhaseCounter() const;
	const FrameCounter& UpdatePhaseCounter() const;

private:
	float timeStep;
	float partitioningSize;
	EcsWorld& ecs;
	IConstraint& constraint;
	PartitioningGrid partitioning;
	FrameCounter broadPhaseCounter = FrameCounter(0.25f);
	FrameCounter narrowPhaseCounter = FrameCounter(0.25f);
	FrameCounter updatePhaseCounter = FrameCounter(0.25f);
	ThreadPool threadPool = {};

	void CollectStats();
	void Simulate(float dt);
	void Collisions();
	void SolveCell(PartitioningCell& cell);
	void SolveCells(PartitioningCell& cell0, PartitioningCell& cell1);
	void Solve(Transform& aTransform, Particle& a, Transform& bTransform, Particle& b);
	void UpdateObjects(float dt);
};
