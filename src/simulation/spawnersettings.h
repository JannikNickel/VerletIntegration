#pragma once
#include "structs/vector2.h"
#include "structs/color.h"

enum class SpawnColorMode
{
	Fixed,
	RandomHue,
	ShiftHue,
	//TODO random gradient and shift gradient
};

enum class SpawnRepeatMode
{
	Repeat,
	Reverse
};

enum class SpawnDirectionMode
{
	Fixed,
	RotateCW,
	RotateCCW,
	//TODO are the options above useful
};

struct SpawnerSettings
{
	float spawnRate = 0.5f;
	bool scaleSpawnRate = true;

	Vector2 pSize = Vector2(3.0f, 7.5f);
	float pMass = 1.0f;
	bool scalePMass = true;
	float pBounciness = 0.1f;

	SpawnColorMode pColorMode = SpawnColorMode::Fixed;
	Color pColorSingle = Color::white;
	SpawnRepeatMode pColorShift = SpawnRepeatMode::Repeat;
	float pColorShiftDuration = 10.0f;

	float spawnDirection = 0.0f;
	float spawnDirectionVariation = 0.0f;
	Vector2 spawnForce = Vector2(350.0f, 350.0f);
	bool scaleSpawnForce = true;
	SpawnDirectionMode spawnDirectionMode = SpawnDirectionMode::Fixed;
	SpawnRepeatMode spawnDirectionRepeatMode = SpawnRepeatMode::Reverse;
	float spawnDirectionDuration = 5.0f;

	Vector2 SpawnDirVector(float offset = 0.0f) const { return Vector2::Rotate(Vector2(0.0f, -1.0f), spawnDirection + offset); };
};
