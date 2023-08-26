#include <iostream>
#include <algorithm>

#include "engine/window.h"
#include "utils/framecounter.h"
#include "editor/editor.h"

#include "imgui.h"

static const float windowSize = 1080;

int main()
{
	Window window = Window(static_cast<unsigned int>(windowSize), static_cast<unsigned int>(windowSize), "Verlet Integration", 6000, 100, true);
	FrameCounter frameCounter = FrameCounter(0.5);

	Editor editor = Editor();

	window.Show([&](double dt)
	{
		frameCounter.Frame(dt);

		editor.Update(dt);

		/*if(ImGui::BeginMainMenuBar())
		{
			ImGui::SetNextItemWidth(size);
			ImGui::LabelText("", "FPS = %d, OBJECTS = %d, SIMULATION = %.2f ms, RENDERPREP = %.2f ms, RENDER = %.2f ms", (int)frameCounter.Framerate(), physicsObjCount, physicsCounter.Frametime() * 1000, renderCounter.Frametime() * 1000, window.LastFrameRenderTime());
			ImGui::EndMainMenuBar();
		}
		ImGui::SetNextWindowPos({ 5.0f, 25.0f });
		ImGui::SetNextWindowBgAlpha(0.25f);
		if(ImGui::Begin("Solver", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::LabelText("", "Broad phase = %.2f ms", solver.BroadPhaseCounter().Frametime() * 1000.0);
			ImGui::LabelText("", "Narrow phase = %.2f ms", solver.NarrowPhaseCounter().Frametime() * 1000.0);
			ImGui::LabelText("", "Update phase = %.2f ms", solver.UpdatePhaseCounter().Frametime() * 1000.0);
			ImGui::End();
		}*/
	});

}
