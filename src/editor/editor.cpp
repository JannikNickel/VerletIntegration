#include "editor.h"
#include "guihelper.h"
#include "imgui.h"
#include <algorithm>
#include <cstdint>

void Editor::Update(double dt)
{
	/*ImGui::ShowDemoWindow();
	return;*/

	MainMenuBar();

	if(currentPopup != nullptr)
	{
		(*currentPopup)();
	}

	Render();
}

void Editor::Render()
{
	if(world != nullptr)
	{
		world->Render();
	}
}

void Editor::OpenScene(std::shared_ptr<Scene> scene)
{
	this->scene = scene;
	this->world = scene->CreateWorld(Color::From32(30, 30, 30), Color::From32(15, 15, 15));
	//TODO load scene
}

void Editor::MainMenuBar()
{
	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			FileMenu();
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Add"))
		{
			AddMenu();
			ImGui::EndMenu();
		}
		if(ImGui::MenuItem("Simulate"))
		{

		}

		ImGui::EndMainMenuBar();
	}
}

void Editor::FileMenu()
{
	if(ImGui::MenuItem("New Simulation", "Ctrl+N"))
	{
		currentPopup = std::make_unique<std::function<void()>>([this]()
		{
			NewSimulationPopup();
		});
	}
	if(ImGui::MenuItem("Open", "Ctrl+O"))
	{

	}
	if(ImGui::BeginMenu("Open Recent"))
	{
		ImGui::MenuItem("0.json");
		ImGui::MenuItem("1.json");
		ImGui::MenuItem("2.json");
		ImGui::EndMenu();
	}
	if(ImGui::MenuItem("Save", "Ctrl+S"))
	{

	}
	if(ImGui::MenuItem("Save As..."))
	{

	}
}

void Editor::AddMenu()
{
	if(ImGui::MenuItem("Particle", ""))
	{

	}
	if(ImGui::MenuItem("Particle Spawner", ""))
	{

	}
	if(ImGui::MenuItem("Link", ""))
	{

	}
	if(ImGui::MenuItem("Spring", ""))
	{

	}
}

void Editor::NewSimulationPopup()
{
	static const int32_t minSceneSize = 128;
	static const int32_t maxSceneSize = 1080;
	static int32_t sceneSize = 1080;
	static WorldData worldData = { WorldShape::Rect, { Vector2(sceneSize, sceneSize) } };

	GuiHelper::CenterNextWindow();
	ImGui::OpenPopup("New Simulation");
	if(ImGui::BeginPopupModal("New Simulation", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		bool clamp = false;

		ImGui::LabelText("", "Scene Size");
		if(ImGui::InputInt("##sizeInput", &sceneSize, 0, 0))
		{
			sceneSize = std::clamp(sceneSize, minSceneSize, maxSceneSize);
		}
		//TODO also set clamp after editing this

		ImGui::LabelText("", "World Shape");
		if(GuiHelper::EnumDropdown("##worldShapeCombo", &worldData.shape))
		{
			clamp = true;
			worldData.bounds.size = Vector2(sceneSize, sceneSize);
		}

		switch(worldData.shape)
		{
			case WorldShape::Rect:
			{
				ImGui::LabelText("", "World Size");
				if(ImGui::InputFloat2("##worldSizeInput", &worldData.bounds.size[0], "%.0f") || clamp)
				{
					worldData.bounds.size.x = std::clamp(worldData.bounds.size.x, 64.0f, static_cast<float>(sceneSize));
					worldData.bounds.size.y = std::clamp(worldData.bounds.size.y, 64.0f, static_cast<float>(sceneSize));
				}
				break;
			}
			case WorldShape::Circle:
			{
				ImGui::LabelText("", "World Radius");
				if(ImGui::InputFloat("##worldSizeInput", &worldData.bounds.radius, 0.0f, 0.0f, "%.0f") || clamp)
				{
					worldData.bounds.radius = std::clamp(worldData.bounds.radius, 32.0f, static_cast<float>(sceneSize / 2));
				}
				break;
			}
			default:
				throw std::exception("Missing world shape label!");
		}

		ImGui::Spacing();
		int result = GuiHelper::HorizontalButtonSplit("Create", "Cancel");
		if(result == 1)
		{
			OpenScene(std::make_shared<Scene>(sceneSize, worldData));
		}
		if(result > 0)
		{
			ImGui::CloseCurrentPopup();
			currentPopup = nullptr;
		}

		ImGui::EndPopup();
	}
}
