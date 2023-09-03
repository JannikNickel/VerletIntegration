#pragma once
#include "components.h"
#include "spawner.h"
#include "core/world.h"
#include "physics/verletsolver.h"
#include "ecs/world.h"
#include <memory>
#include <vector>
#include <cstdint>

class Simulation
{
public:
	Simulation(std::unique_ptr<World> world, const SolverSettings& solverSettings);

	void Update(double dt);
	void Render();

	void AddParticle(Particle&& particle, Vector2 pos, const Color& color);
	void AddSpawner(Spawner&& spawner);

	uint32_t ParticleAmount() const;

private:
	std::unique_ptr<World> world;
	std::unique_ptr<EcsWorld> ecs;
	std::unique_ptr<VerletSolver> solver;

	std::vector<Spawner> spawners = {};
	uint32_t particleAmount = 0;
};
