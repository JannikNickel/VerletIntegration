#pragma once
#include "components.h"
#include "spawner.h"
#include "core/world.h"
#include "physics/verletsolver.h"
#include "ecs/world.h"
#include <memory>
#include <vector>

class Simulation
{
public:
	Simulation(std::unique_ptr<World> world, const SolverSettings& solverSettings);

	void Update(double dt);
	void Render();

	void AddParticle(Particle&& particle, Vector2 pos, const Color& color);
	void AddSpawner(Spawner&& spawner);

private:
	std::unique_ptr<World> world;
	std::unique_ptr<EcsWorld> ecs;
	std::unique_ptr<VerletSolver> solver;

	std::vector<Spawner> spawners = {};
};
