#pragma once
#include "spawnersettings.h"
#include "utils/random.h"
#include <functional>
#include <cstdint>

class Spawner
{
public:
	Spawner(Vector2 position, const SpawnerSettings& settings) : position(position), settings(settings), time(settings.spawnRate) { }

	void Update(class Simulation& simulation, float dt);

private:
	Vector2 position;
	SpawnerSettings settings;

	Random random = {};
	uint32_t spawned = 0;
	float time = 0.0f;
	float spawnTimer = 0.0f;
	Vector2 direction = Vector2::zero;

	bool CanSpawn(Simulation& simulation) const;
	float SpawnParticle(Simulation& simulation);
	Color ParticleColor();
	float RotationOffset() const;
};
