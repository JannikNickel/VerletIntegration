#include "editor.h"
#include "guihelper.h"
#include "renderer/graphics.h"
#include "imgui.h"
#include <algorithm>
#include <cstdint>

struct SimulationPopupData
{
	int32_t sceneSize = 1080;
	WorldData worldData = {	WorldShape::Rect, { Vector2(sceneSize, sceneSize) }, Color::From32(30, 30, 30),	Color::From32(15, 15, 15) };
};

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
	this->world = scene->CreateWorld();

	Graphics::SetProjection(scene->Size(), scene->Size());

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
		currentPopup = std::make_unique<std::function<void()>>([this, data = SimulationPopupData()]() mutable
		{
			NewSimulationPopup(data);
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

void Editor::NewSimulationPopup(SimulationPopupData& data)
{
	static const int32_t minSceneSize = 128;
	static const int32_t maxSceneSize = 1080;

	GuiHelper::CenterNextWindow();
	ImGui::OpenPopup("New Simulation");
	if(ImGui::BeginPopupModal("New Simulation", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::LabelText("", "Scene Size");
		if(ImGui::InputInt("##sizeInput", &data.sceneSize, 0, 0))
		{
			data.sceneSize = std::clamp(data.sceneSize, minSceneSize, maxSceneSize);
		}

		ImGui::LabelText("", "World Shape");
		if(GuiHelper::EnumDropdown("##worldShapeCombo", &data.worldData.shape))
		{
			data.worldData.bounds.size = Vector2(data.sceneSize, data.sceneSize);
		}

		bool clamp = !ImGui::IsAnyItemActive();
		switch(data.worldData.shape)
		{
			case WorldShape::Rect:
			{
				ImGui::LabelText("", "World Size");
				if(ImGui::InputFloat2("##worldSizeInput", &data.worldData.bounds.size[0], "%.0f") || clamp)
				{
					data.worldData.bounds.size.x = std::clamp(data.worldData.bounds.size.x, 64.0f, static_cast<float>(data.sceneSize));
					data.worldData.bounds.size.y = std::clamp(data.worldData.bounds.size.y, 64.0f, static_cast<float>(data.sceneSize));
				}
				break;
			}
			case WorldShape::Circle:
			{
				ImGui::LabelText("", "World Radius");
				if(ImGui::InputFloat("##worldSizeInput", &data.worldData.bounds.radius, 0.0f, 0.0f, "%.0f") || clamp)
				{
					data.worldData.bounds.radius = std::clamp(data.worldData.bounds.radius, 32.0f, static_cast<float>(data.sceneSize / 2));
				}
				break;
			}
			default:
				throw std::exception("Missing world shape label!");
		}

		ImGui::LabelText("", "Scene Background");
		ImGui::ColorEdit3("##sceneBackgroundInput", &data.worldData.background.r, ImGuiColorEditFlags_DisplayHex);

		ImGui::LabelText("", "World Color");
		ImGui::ColorEdit3("##worldColorInput", &data.worldData.color.r, ImGuiColorEditFlags_DisplayHex);

		ImGui::Spacing();
		int result = GuiHelper::HorizontalButtonSplit("Create", "Cancel");
		if(result == 1)
		{
			OpenScene(std::make_shared<Scene>(data.sceneSize, data.worldData));
		}
		if(result > 0)
		{
			ImGui::CloseCurrentPopup();
			currentPopup = nullptr;
		}

		ImGui::EndPopup();
	}
}
