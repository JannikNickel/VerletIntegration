#include <iostream>
#include <algorithm>

#include "engine/window.h"
#include "engine/input.h"
#include "renderer/graphics.h"
#include "core/world.h"
#include "core/circleworld.h"
#include "core/rectworld.h"
#include "utils/framecounter.h"
#include "physics/verletsolver.h"
#include "ecs/world.h"
#include "simulation/simulation.h"

#include "imgui.h"

int main()
{
	const float size = 720.0f;
	const float physicsSps = 60.0f;
	const float gravity = -900.0f;
	const unsigned int substeps = 8;
	const float minParticleSize = 3.5f;
	const float maxParticleSize = 10.0f;
	const bool circleWorld = false;

	Window window = Window(static_cast<unsigned int>(size), static_cast<unsigned int>(size), "Verlet Integration", -1, -1, true);
	World* world = circleWorld ?
		static_cast<World*>(new CircleWorld(Color::From32(30, 30, 30), Vector2::one * size * 0.5f, size * 0.45f, Color::From32(15, 15, 15, 255)))
		: static_cast<World*>(new RectWorld(Color::From32(30, 30, 30), Vector2::one * size * 0.5f, Vector2::one * size, Color::From32(15, 15, 15, 255)));
	EcsWorld ecs = EcsWorld();
	FrameCounter frameCounter = FrameCounter(0.5);
	FrameCounter renderCounter = FrameCounter(0.25);
	FrameCounter physicsCounter = FrameCounter(0.25);
	VerletSolver solver = VerletSolver(ecs, dynamic_cast<IConstraint*>(world), 1.0f / physicsSps, gravity, substeps, maxParticleSize * 2.5f);
	solver.collision = true;
	solver.updateMode = SolverUpdateMode::FixedFrameRate;

	const int spawnAmount = 16;
	double spawnCooldown[spawnAmount] = { 0.0f };
	double time = 0.0f;
	size_t physicsObjCount = 0;

	window.Show([&](double dt)
	{
		time += dt;
		frameCounter.Frame(dt);
		
		for(size_t i = 0; i <= spawnAmount; i++)
		{
			spawnCooldown[i] -= dt;
			if(spawnCooldown[i] <= 0.0f && Input::KeyHeld(KeyCode::Enter))
			{
				float r = std::clamp(rand() / (float)RAND_MAX * maxParticleSize, minParticleSize, maxParticleSize);
				float m = 1.0f;
				Color col = Color::FromHSV(rand() / (float)RAND_MAX, 0.75f, 0.75f);
				Vector2 pos = world->Center() + Vector2((static_cast<float>(i) - spawnAmount / 2.0f) * 15.0f, size * 0.25f);
				Vector2 acc = Vector2(std::sinf(time), -0.33f * (spawnAmount > 1 ? 2.0f : 1.0f)).Normalized() * 500000.0f;

				spawnCooldown[i] = 0.015f * r;
				ecs.CreateEntity(Transform(Matrix4::PositionScale2d(pos, r * 2.0f)), RenderColor(col), PhysicsCircle(r, m, pos, acc));
				physicsObjCount++;
			}
		}

		physicsCounter.BeginFrame();
		solver.Update(dt);
		physicsCounter.EndFrame();

		renderCounter.BeginFrame();
		world->Render();
		int iIndex = 0;
		ecs.QueryChunked<Transform, RenderColor>(Graphics::instancingLimit, [](Transform* transform, RenderColor* renderColor, size_t chunkSize)
		{
			Graphics::CirclesInstanced(reinterpret_cast<const Matrix4*>(transform), reinterpret_cast<const Color*>(renderColor), chunkSize);
		});
		renderCounter.EndFrame();

		if(ImGui::BeginMainMenuBar())
		{
			ImGui::SetNextItemWidth(size);
			ImGui::LabelText("", "FPS = %d, OBJECTS = %d, SIMULATION = %.2f ms, RENDERPREP = %.2f ms, RENDER = %.2f ms", (int)frameCounter.Framerate(), physicsObjCount, physicsCounter.Frametime() * 1000, renderCounter.Frametime() * 1000, window.LastFrameRenderTime());
			ImGui::EndMainMenuBar();
		}
	});

	delete world;
}
