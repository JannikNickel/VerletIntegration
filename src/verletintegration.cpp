#include <iostream>
#include <algorithm>
#include <chrono>

#include "window.h"
#include "input.h"
#include "graphics.h"
#include "verletphysics.h"

#include "imgui.h"

int main()
{
	const float size = 720.0f;
	Window window = Window(static_cast<unsigned int>(size), static_cast<unsigned int>(size), "Verlet Integration");

	const double fpsUpdateRate = 2.0;
	int fpsFrameCount = 0;
	double fpsFrameTime = 0.0;
	int fps = 0;

	VerletPhysics physics = VerletPhysics();
	physics.gravity = -900.0f;
	physics.center = Vector2::one * size * 0.5f;
	physics.radius = size * 0.45f;

	double spawnCooldown = 0.0f;
	double time = 0.0f;

	window.Show([&](double dt)
	{
		time += dt;
		fpsFrameCount++;
		fpsFrameTime += dt;
		if(fpsFrameTime > 1.0 / fpsUpdateRate)
		{
			fps = fpsFrameCount * fpsUpdateRate;
			fpsFrameCount = 0;
			fpsFrameTime -= 1.0 / fpsUpdateRate;
		}

		spawnCooldown -= dt;
		if(spawnCooldown <= 0.0f && Input::KeyHeld(KeyCode::Enter))
		{
			for(int i = 0; i < 10; i++)
			{
				float r = std::clamp(rand() / (float)RAND_MAX * 10.0f, 3.5f, 10.0f);
				//spawnCooldown = 0.015f * r;
				VerletObj obj = VerletObj(0, physics.center + Vector2(0.0f, size * 0.25f), 1.0f, r, Color::FromHSV(rand() / (float)RAND_MAX, 0.75f, 0.75f));
				Vector2 dir = Vector2(std::sinf(time), -0.33f);
				obj.acc = dir.Normalized() * 500000.0f;
				physics.objects.push_back(obj);
			}
		}

		auto t0 = std::chrono::high_resolution_clock::now();
		physics.Update(dt);
		auto t1 = std::chrono::high_resolution_clock::now();
		Graphics::Circle(Vector2(physics.center), physics.radius, Color::From32(15, 15, 15, 255));
		for(const VerletObj& obj : physics.objects)
		{
			Graphics::Circle(obj.pos, obj.radius, obj.color);
		}
		auto t2 = std::chrono::high_resolution_clock::now();

		if(ImGui::BeginMainMenuBar())
		{
			float simulation = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() * 1e-6f;
			float render = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() * 1e-6f;
			ImGui::SetNextItemWidth(size);
			ImGui::LabelText("", "FPS = %d, OBJECTS = %d, SIMULATION = %.2f ms, RENDERPREP = %.2f ms, RENDER = %.2f ms", (int)fps, physics.objects.size(), simulation, render, window.LastFrameRenderTime());
			ImGui::EndMainMenuBar();
		}
	});
}
