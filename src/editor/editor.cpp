#include "editor.h"
#include "guihelper.h"
#include "particleobject.h"
#include "renderer/graphics.h"
#include "engine/input.h"
#include "imgui.h"
#include <algorithm>
#include <cstdint>
#include <ranges>

static const Color previewValidColor = Color::From32(53, 132, 222, 128);
static const Color previewInvalidColor = Color::From32(222, 53, 53, 128);
static const Color sceneObjectColor = Color::From32(255, 255, 255, 200);
static const Color selectedColor = Color::From32(222, 152, 53, 255);
static const Color selectedHoverColor = Color::From32(222, 183, 111, 255);

struct SimulationPopupData
{
	int32_t sceneSize = 1080;
	WorldData worldData = {	WorldShape::Rect, { Vector2(sceneSize, sceneSize) }, Color::From32(30, 30, 30), Color::From32(15, 15, 15) };
};

Editor::Editor(const std::shared_ptr<Window>& window) : window(window)
{

}

void Editor::Update(double dt)
{
	static bool init = false;
	if(!init)
	{
		init = true;
		OpenScene(std::make_shared<Scene>(1080, WorldData { WorldShape::Circle, { .radius = 500.0f }, Color::From32(30, 30, 30), Color::From32(15, 15, 15) }));
	}

	Render(dt);
	UI();
	Selection();
	SelectionInteraction();
	Placement(dt);
}

void Editor::Render(double dt)
{
	if(world != nullptr)
	{
		world->Render();
	}

	std::shared_ptr<SceneObject> selected = currentSelected.lock();
	std::shared_ptr<SceneObject> hovered = currentHovered.lock();
	for(const std::shared_ptr<SceneObject>& obj : scene->Objects())
	{
		obj->Render(dt, selected == obj ? std::make_optional(selectedColor) : (hovered == obj ? std::make_optional(selectedHoverColor) : std::nullopt));
	}
}

void Editor::UI()
{
	MainMenuBar();
	if(currentPopup != nullptr)
	{
		(*currentPopup)();
	}
}

void Editor::Placement(double dt)
{
	if(currentPreview != nullptr)
	{
		Vector2 pos = Input::MousePosition();
		bool valid = world->Contains(pos) && GetHoveredObject().expired();
		currentPreview->position = pos;
		currentPreview->Render(dt, valid ? previewValidColor : previewInvalidColor);
		if(valid && Input::KeyPressed(KeyCode::LMB))
		{
			scene->AddObject(std::move(currentPreview));
		}
	}
}

void Editor::Selection()
{
	if(currentPreview != nullptr)
	{
		return;
	}

	currentHovered = GetHoveredObject();
	if(Input::KeyPressed(KeyCode::LMB) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	{
		SelectObject(currentHovered);
	}
}

void Editor::SelectionInteraction()
{
	if(std::shared_ptr<SceneObject> selected = currentSelected.lock())
	{
		Vector2 pos = selected->position;
		float xDir = pos.x / static_cast<float>(scene->Size()) > 0.75f ? -1.0f : 1.0f;
		ImGui::SetNextWindowPos({ pos.x + xDir * 25.0f, window->Size().y - pos.y - GuiHelper::TitleBarHeight() * 0.5f }, ImGuiCond_Appearing, { (-xDir + 1.0f) * 0.5f, 0.0f });
		EditResult result = EditResult::None;
		if(ImGui::Begin(selected->ObjectName(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			result = selected->Edit();
			ImGui::End();
		}

		if(result == EditResult::Delete || Input::KeyPressed(KeyCode::Delete))
		{
			scene->RemoveObject(selected);
		}
		else if(result == EditResult::Duplicate)
		{
			CreatePreview(selected->Clone());
		}
	}
}

void Editor::OpenScene(const std::shared_ptr<Scene>& scene)
{
	this->scene = scene;
	this->world = scene->CreateWorld();

	Graphics::SetProjection(scene->Size(), scene->Size());
}

void Editor::CreatePreview(std::unique_ptr<SceneObject>&& obj)
{
	SelectObject({});
	currentPreview = std::move(obj);
}

void Editor::SelectObject(const std::weak_ptr<SceneObject>& obj)
{
	currentSelected = obj;
}

std::weak_ptr<SceneObject> Editor::GetHoveredObject()
{
	Vector2 mousePos = Input::MousePosition();
	for(const std::shared_ptr<SceneObject>& obj : scene->Objects() | std::views::reverse)
	{
		if(obj->IsHovered(mousePos))
		{
			return obj;
		}
	}
	return std::weak_ptr<SceneObject>();
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
		CreatePreview(std::make_unique<ParticleObject>(Vector2::zero));
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
