#pragma once
#include "structs/vector2.h"
#include <cstdint>

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

struct SolverSettings
{
	SolverUpdateMode updateMode = SolverUpdateMode::FixedFrameRate;
	float timestep = 1.0f / 60.0f;
	uint32_t substeps = 8;

	Vector2 gravity = Vector2(0.0f, -900.0f);
	float partitioningSize = 25.0f;
	bool collision = true;

	SolverSettings(SolverUpdateMode updateMode, float timestep, uint32_t substeps, Vector2 gravity, float partitioningSize, bool collision)
		: updateMode(updateMode), timestep(timestep), substeps(substeps), gravity(gravity), partitioningSize(partitioningSize), collision(collision) { }
	SolverSettings() = default;
};
