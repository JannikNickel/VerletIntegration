#include <iostream>
#include <memory>
#include <optional>

#include "engine/window.h"
#include "utils/framecounter.h"
#include "editor/editor.h"
#include "simulation/simulation.h"
#include "engine/input.h"
#include "imgui.h"

static const float windowSize = 1080;

void DrawStats(const FrameCounter& frameCounter, const Simulation& simulation, float renderTime);

int main()
{
	std::shared_ptr<Window> window = std::make_shared<Window>(static_cast<unsigned int>(windowSize), static_cast<unsigned int>(windowSize), "Verlet Integration", -1, -1, true);
	FrameCounter frameCounter = FrameCounter(0.5);
	std::optional<Simulation> simulation = std::nullopt;
	bool showStats = false;
	Editor editor = Editor(window, [&](Simulation sim)
	{
		simulation.emplace(std::move(sim));
	});

	window->Show([&](double dt)
	{
		frameCounter.Frame(dt);

		if(simulation.has_value())
		{
			simulation->Update(dt);
			simulation->Render();
			
			if(showStats = (Input::KeyPressed(KeyCode::Space) ? !showStats : showStats))
			{
				DrawStats(frameCounter, simulation.value(), window->LastFrameRenderTime());
			}

			if(Input::KeyPressed(KeyCode::Escape))
			{
				simulation.reset();
			}
		}
		else
		{
			editor.Update(dt);
		}
	});
}

void DrawStats(const FrameCounter& frameCounter, const Simulation& simulation, float renderTime)
{
	ImGui::SetNextWindowPos({ 5.0f, 5.0f });
	ImGui::SetNextWindowBgAlpha(0.25f);
	if(ImGui::Begin("##stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text("FPS:     %.2f", frameCounter.Framerate());
		ImGui::Text("Frame:   %.2f ms", frameCounter.Frametime() * 1000.0);
		ImGui::Text("Render:  %.2f ms", renderTime);
		ImGui::Separator();
		ImGui::Text("Objects: %d", simulation.ParticleAmount());
		ImGui::Text("Broad:   %.2f ms", simulation.Solver().BroadPhaseCounter().Frametime() * 1000.0);
		ImGui::Text("Narrow:  %.2f ms", simulation.Solver().NarrowPhaseCounter().Frametime() * 1000.0);
		ImGui::Text("Update:  %.2f ms", simulation.Solver().UpdatePhaseCounter().Frametime() * 1000.0);
		ImGui::Text("Link:    %.2f ms", simulation.Solver().LinkPhaseCounter().Frametime() * 1000.0);
		ImGui::End();
	}
}
