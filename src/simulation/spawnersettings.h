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

enum class SpawnDirectionRotation
{
	Fixed,
	RotateLinear,
	RotateSmoothStep,
	RotateSmootherStep
};

enum class SpawnCondition
{
	Always,
	Duration,
	LocalAmount,
	GlobalAmount
};

struct SpawnerSettings
{
	float spawnRate = 0.5f;
	bool scaleSpawnRate = true;
	float initialDelay = 0.0f;
	SpawnCondition spawnCondition = SpawnCondition::Always;
	float spawnConditionValue = 1000.0f;

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
	SpawnDirectionRotation spawnDirectionRotation = SpawnDirectionRotation::Fixed;
	SpawnRepeatMode spawnDirectionRotationRepeat = SpawnRepeatMode::Reverse;
	float spawnDirectionRotationStart = 0.5f;
	float spawnDirectionRotationDuration = 5.0f;
	float spawnDirectionRotationLimit = 360.0f;

	Vector2 SpawnDirVector(float offset = 0.0f) const
	{
		return Vector2::Rotate(Vector2(0.0f, -1.0f), spawnDirection + offset);
	};
};
