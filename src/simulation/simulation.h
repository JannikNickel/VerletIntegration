#pragma once
#include "components.h"
#include "spawner.h"
#include "core/world.h"
#include "physics/verletsolver.h"
#include "ecs/world.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>

class Simulation
{
public:
	Simulation(std::unique_ptr<World> world, const SolverSettings& solverSettings);

	void Update(double dt);
	void Render();

	void AddParticle(Particle&& particle, Vector2 pos, const Color& color, uint32_t objId = 0);
	void AddSpawner(Spawner&& spawner, uint32_t objId = 0);
	void AddLink(Link&& link, uint32_t p0Id, uint32_t p1Id, const Color& color, uint32_t objId = 0);
	void AddForceField(ForceField&& forceField, uint32_t objId);

	uint32_t ParticleAmount() const { return particleAmount; }
	const VerletSolver& Solver() const { return *solver; }

private:
	std::unique_ptr<World> world;
	std::unique_ptr<EcsWorld> ecs;
	std::unique_ptr<VerletSolver> solver;

	std::unordered_map<uint32_t, Entity> placedEntityMap = {};
	std::vector<Spawner> spawners = {};
	uint32_t particleAmount = 0;
};
