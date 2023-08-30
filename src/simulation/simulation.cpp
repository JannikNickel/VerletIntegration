#include "simulation.h"
#include "renderer/graphics.h"

Simulation::Simulation(std::unique_ptr<World> world) : world(std::move(world))
{
	ecs = std::make_unique<EcsWorld>();
	solver = std::make_unique<VerletSolver>(*ecs.get(), static_cast<IConstraint&>(*this->world.get()), 1.0f / 60.0f, -900.0f, 8, 10.0f * 2.0f);
	solver->updateMode = SolverUpdateMode::FixedFrameRate;
}

void Simulation::Update(double dt)
{
	solver->Update(dt);
}

void Simulation::Render()
{
	world->Render();
	ecs->QueryChunked<Transform, RenderColor>(Graphics::instancingLimit, [](Transform* transform, RenderColor* renderColor, size_t chunkSize)
	{
		Graphics::CirclesInstanced(reinterpret_cast<const Matrix4*>(transform), reinterpret_cast<const Color*>(renderColor), chunkSize);
	});
}

void Simulation::AddParticle(Particle&& particle, Vector2 pos, const Color& color)
{
	particle.prevPos = pos;
	ecs->CreateEntity(Transform(Matrix4::PositionScale2d(pos, particle.radius * 2.0f)), RenderColor(color), std::move(particle));
}
