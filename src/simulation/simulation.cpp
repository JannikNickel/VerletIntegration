#include "simulation.h"
#include "renderer/graphics.h"

Simulation::Simulation(std::unique_ptr<World> world, const SolverSettings& solverSettings) : world(std::move(world))
{
	ecs = std::make_unique<EcsWorld>();
	solver = std::make_unique<VerletSolver>(*ecs.get(), static_cast<IConstraint&>(*this->world.get()), solverSettings);
}

void Simulation::Update(double dt)
{
	for(Spawner& spawner : spawners)
	{
		spawner.Update(*this, dt);
	}

	solver->Update(dt);
}

void Simulation::Render()
{
	world->Render();
	ecs->QueryChunked<Transform, RenderColor, Link>(Graphics::instancingLimit, [](Transform* transform, RenderColor* renderColor, Link* _, size_t chunkSize)
	{
		Graphics::LinesInstanced(reinterpret_cast<const Matrix4*>(transform), reinterpret_cast<const Color*>(renderColor), chunkSize);
	});
	ecs->QueryChunked<Transform, RenderColor, Particle>(Graphics::instancingLimit, [](Transform* transform, RenderColor* renderColor, Particle* _, size_t chunkSize)
	{
		Graphics::CirclesInstanced(reinterpret_cast<const Matrix4*>(transform), reinterpret_cast<const Color*>(renderColor), chunkSize);
	});
}

void Simulation::AddParticle(Particle&& particle, Vector2 pos, const Color& color, uint32_t objId)
{
	particle.prevPos = pos;
	Entity entity = ecs->CreateEntity(Transform(Matrix4::PositionScale2d(pos, particle.radius * 2.0f)), RenderColor(color), std::move(particle));
	if(objId != 0)
	{
		placedEntityMap.emplace(objId, entity);
	}
	particleAmount++;
}

void Simulation::AddSpawner(Spawner&& spawner, uint32_t objId)
{
	spawners.push_back(std::move(spawner));
}

void Simulation::AddLink(Link&& link, uint32_t p0Id, uint32_t p1Id, const Color& color, uint32_t objId)
{
	link.e0 = placedEntityMap[p0Id];
	link.e1 = placedEntityMap[p1Id];
	link.distance = Vector2::Distance(ecs->GetComponent<Transform>(link.e0)->Position(), ecs->GetComponent<Transform>(link.e1)->Position());
	Entity entity = ecs->CreateEntity(Transform(Matrix4::identity), RenderColor(color), std::move(link));
	if(objId != 0)
	{
		placedEntityMap.emplace(objId, entity);
	}
}
