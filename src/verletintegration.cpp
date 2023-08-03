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

struct Transform4b : Componentb<Transform4b>
{
	Matrix4 value;
};

struct Transform4 : Component<Transform4>
{
	Matrix4 value;

	Transform4(Matrix4 value) : value(value) { }
};

struct RenderColor : Component<RenderColor>
{
	Color value;

	RenderColor(Color value) : value(value) { }
};

struct PhysicsCircle : Component<PhysicsCircle>
{
	float radius;
	float mass;
	Vector2 prevPos;
	Vector2 acceleration;

	PhysicsCircle(float radius, float mass, Vector2 prevPos, Vector2 acceleration) : radius(radius), mass(mass), prevPos(prevPos), acceleration(acceleration) { }
};

int main()
{
	const float size = 720.0f;
	const float physicsSps = 144.0f;
	const float gravity = -900.0f;
	const unsigned int substeps = 8;

	Window window = Window(static_cast<unsigned int>(size), static_cast<unsigned int>(size), "Verlet Integration");
	World* world = new CircleWorld(Color::From32(30, 30, 30), Vector2::one * size * 0.5f, size * 0.45f, Color::From32(15, 15, 15, 255));
	EcsWorld ecs = EcsWorld();
	FrameRateCounter frameRate = FrameRateCounter();
	VerletSolver solver = VerletSolver(1.0f / physicsSps, dynamic_cast<IConstraint*>(world), gravity, substeps);


	double spawnCooldown = 0.0f;
	double time = 0.0f;
	size_t physicsObjCount = 0;

	window.Show([&](double dt)
	{
		time += dt;
		frameRate.Frame(dt);

		spawnCooldown -= dt;
		if(spawnCooldown <= 0.0f && Input::KeyHeld(KeyCode::Enter))
		{
			float r = std::clamp(rand() / (float)RAND_MAX * 10.0f, 3.5f, 10.0f) * 5;
			float m = 1.0f;
			Color col = Color::FromHSV(rand() / (float)RAND_MAX, 0.75f, 0.75f);
			Vector2 pos = world->Center() + Vector2(0.0f, size * 0.25f);
			Vector2 acc = Vector2(std::sinf(time), -0.33f).Normalized() * 500000.0f;

			spawnCooldown = 0.015f * r;
			ecs.CreateEntity(Transform4(Matrix4::PositionScale2d(pos, r)), RenderColor(col), PhysicsCircle(r, m, pos, acc));
			physicsObjCount++;
			ecs.Query<RenderColor>([](RenderColor& tc) { });
		}

		auto t0 = Clock::now();
		//solver.Update(dt);
		auto t1 = Clock::now();
		world->Render();
		int iIndex = 0;
		ecs.QueryChunked<Transform4, RenderColor>(Graphics::instancingLimit, [](Transform4* transform, RenderColor* renderColor, size_t chunkSize)
		{
			//THIS IS A PROBLEM, because the vtable increases the struct size of Transform4 vs Matrix4 (72 vs 64)
			std::cout << sizeof(Transform4b) << " == " << sizeof(Matrix4) << std::endl;
			Matrix4* mats = (Matrix4*)transform;
			std::cout << chunkSize << "\n";
			for(size_t i = 0; i < chunkSize; i++)
			{
				std::cout << "pos = " << mats[i].GetPosition() << ", scale = " << mats[i].GetScale() << std::endl;
			}

			Graphics::CirclesInstanced(mats, reinterpret_cast<Color*>(renderColor), chunkSize);
		});
		auto t2 = Clock::now();

		if(ImGui::BeginMainMenuBar())
		{
			float simulation = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() * 1e-6f;
			float render = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() * 1e-6f;
			ImGui::SetNextItemWidth(size);
			ImGui::LabelText("", "FPS = %d, OBJECTS = %d, SIMULATION = %.2f ms, RENDERPREP = %.2f ms, RENDER = %.2f ms", (int)frameRate.Fps(), physicsObjCount, simulation, render, window.LastFrameRenderTime());
			ImGui::EndMainMenuBar();
		}
	});

	delete world;
}
