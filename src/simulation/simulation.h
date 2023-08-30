#pragma once
#include "components.h"
#include "core/world.h"
#include "physics/verletsolver.h"
#include "ecs/world.h"
#include <memory>

class Simulation
{
	friend class Scene;

public:
	Simulation(std::unique_ptr<World> world);

	void Update(double dt);
	void Render();

private:
	std::unique_ptr<World> world;
	std::unique_ptr<EcsWorld> ecs;
	std::unique_ptr<VerletSolver> solver;

	void AddParticle(Particle&& particle, Vector2 pos, const Color& color);
};
