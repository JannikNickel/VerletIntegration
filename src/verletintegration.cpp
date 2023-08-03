#include <iostream>
#include <algorithm>
#include <chrono>

#include "engine/window.h"
#include "engine/input.h"
#include "renderer/graphics.h"
#include "core/world.h"
#include "core/circleworld.h"
#include "core/frameratecounter.h"
#include "physics/verletsolver.h"
#include "ecs/world.h"
#include "ecs/component.h"

#include "imgui.h"

using Clock = std::chrono::high_resolution_clock;

struct Position : public Component<Position>
{
	Vector2 value;

	Position(Vector2 value) : value(value) { }
};

struct RenderColor : public Component<RenderColor>
{
	Color value;

	RenderColor(Color value) : value(value) { }
};

struct Physics : public Component<Physics>
{
	float gravity = 9.81f;
};

int main()
{
	EcsWorld ecs = EcsWorld();
	for(size_t i = 0; i < 10; i++)
	{
		Entity e = ecs.CreateEntity(Position(Vector2(rand() / (double)RAND_MAX * 100.0f, rand() / (double)RAND_MAX * 100.0f)), RenderColor(Color(rand() / (double)RAND_MAX, rand() / (double)RAND_MAX, rand() / (double)RAND_MAX)));
	}
	for(size_t i = 0; i < 5; i++)
	{
		Entity e = ecs.CreateEntity(Position(Vector2(rand() / (double)RAND_MAX, rand() / (double)RAND_MAX)), Physics());
	}
	for(size_t i = 0; i < 5; i++)
	{
		Entity e = ecs.CreateEntity(Position(Vector2(rand() / (double)RAND_MAX * -1.0, rand() / (double)RAND_MAX * -1.0)), RenderColor(Color::black), Physics());
	}
	ecs.Query<Position, RenderColor>([](Position& pos, RenderColor& col)
	{
		std::cout << pos.value.x << ", " << pos.value.y << " | " << col.value.r << ", " << col.value.g << ", " << col.value.b << ", " << col.value.a << std::endl;
	});
	return 0;

	const float size = 720.0f;
	const float physicsSps = 144.0f;
	const float gravity = -900.0f;
	const unsigned int substeps = 8;

	Window window = Window(static_cast<unsigned int>(size), static_cast<unsigned int>(size), "Verlet Integration");
	World* world = new CircleWorld(Color::From32(30, 30, 30), Vector2::one * size * 0.5f, size * 0.45f, Color::From32(15, 15, 15, 255));
	FrameRateCounter frameRate = FrameRateCounter();
	VerletSolver solver = VerletSolver(1.0f / physicsSps, dynamic_cast<IConstraint*>(world), gravity, substeps);

	double spawnCooldown = 0.0f;
	double time = 0.0f;

	Matrix4 iMatrices[Graphics::instancingLimit];
	Color iColors[Graphics::instancingLimit];

	window.Show([&](double dt)
	{
		time += dt;
		frameRate.Frame(dt);

		spawnCooldown -= dt;
		for(size_t i = 0; i < 1000; i++)
		{
			if(spawnCooldown <= 0.0f && Input::KeyHeld(KeyCode::Enter))
			{
				float r = std::clamp(rand() / (float)RAND_MAX * 10.0f, 3.5f, 10.0f);
				//spawnCooldown = 0.015f * r;
				VerletObj obj = VerletObj(solver.objects.size(), world->Center() + Vector2(0.0f, size * 0.25f), 1.0f, r, Color::FromHSV(rand() / (float)RAND_MAX, 0.75f, 0.75f));
				Vector2 dir = Vector2(std::sinf(time), -0.33f);
				obj.acc = dir.Normalized() * 500000.0f;
				solver.objects.push_back(obj);
			}
		}

		auto t0 = Clock::now();
		//solver.Update(dt);
		auto t1 = Clock::now();
		world->Render();
		int iIndex = 0;
		for(VerletObj& obj : solver.objects)
		{
			/*obj.matrix.SetPosition(obj.pos);
			iMatrices[iIndex] = obj.matrix;
			iColors[iIndex] = obj.color;*/
			iIndex++;
			if(iIndex >= Graphics::instancingLimit)
			{
				Graphics::CirclesInstanced(iMatrices, iColors, iIndex);
				iIndex = 0;
			}
		}
		if(iIndex > 0)
		{
			Graphics::CirclesInstanced(iMatrices, iColors, iIndex);
		}
		auto t2 = Clock::now();

		if(ImGui::BeginMainMenuBar())
		{
			float simulation = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() * 1e-6f;
			float render = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() * 1e-6f;
			ImGui::SetNextItemWidth(size);
			ImGui::LabelText("", "FPS = %d, OBJECTS = %d, SIMULATION = %.2f ms, RENDERPREP = %.2f ms, RENDER = %.2f ms", (int)frameRate.Fps(), solver.objects.size(), simulation, render, window.LastFrameRenderTime());
			ImGui::EndMainMenuBar();
		}
	});

	delete world;
}
