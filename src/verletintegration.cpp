#include <iostream>
#include <algorithm>
#include <memory>
#include <optional>

#include "engine/window.h"
#include "utils/framecounter.h"
#include "editor/editor.h"
#include "simulation/simulation.h"
#include "engine/input.h"
#include "utils/math.h"

static const float windowSize = 1080;

int main()
{
	std::shared_ptr<Window> window = std::make_shared<Window>(static_cast<unsigned int>(windowSize), static_cast<unsigned int>(windowSize), "Verlet Integration", -1, -1, true);
	FrameCounter frameCounter = FrameCounter(0.5);
	std::optional<Simulation> simulation = std::nullopt;
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
