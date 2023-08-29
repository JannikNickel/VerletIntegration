#include "editor.h"
#include "guihelper.h"
#include "particleobject.h"
#include "renderer/graphics.h"
#include "engine/input.h"
#include "imgui.h"
#include <algorithm>
#include <cstdint>

static const Color previewValidColor = Color::From32(53, 132, 222, 128);
static const Color previewInvalidColor = Color::From32(222, 53, 53, 128);
static const Color sceneObjectColor = Color::From32(255, 255, 255, 200);
static const Color selectedColor = Color::From32(222, 152, 53, 255);
static const Color selectedHoverColor = Color::From32(222, 183, 111, 255);

static const Color uiErrorColor = Color::From32(255, 53, 53, 200);
static const Color uiSuccessColor = Color::From32(53, 222, 78, 200);

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
	UI(dt);
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

void Editor::UI(double dt)
{
	MainMenuBar();
	if(currentPopup != nullptr)
	{
		(*currentPopup)();
	}
	if(currentNotification.has_value())
	{
		ShowNotification(currentNotification.value());
		if((currentNotification.value().duration -= dt) <= 0.0f)
		{
			SetNotification(std::nullopt);
		}
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
		if(ImGui::Begin(selected->TypeIdentifier(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
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

void Editor::SetPopup(std::unique_ptr<std::function<void()>> popupFunc)
{
	currentPopup = std::move(popupFunc);
}

void Editor::SetNotification(const std::optional<Notification>& notification)
{
	currentNotification = notification;
}

void Editor::OpenScene(const std::shared_ptr<Scene>& scene, const std::optional<FileName>& file)
{
	this->scene = scene;
	this->world = scene->CreateWorld();
	this->currentSaveFile = file;

	Graphics::SetProjection(scene->Size(), scene->Size());
}

void Editor::SaveCurrent(const std::optional<FileName>& file)
{
	if(file.has_value())
	{
		currentSaveFile = file;
	}
	if(currentSaveFile.has_value())
	{
		if(storage.SaveFile(currentSaveFile.value(), scene->Serialize()))
		{
			SetNotification(Notification("Saved scene!", uiSuccessColor.WithAlpha(0.5f)));
			return;
		}
	}
	SetNotification(Notification("Could not save scene!", uiErrorColor.WithAlpha(0.5f)));
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
	if(ImGui::MenuItem("New Simulation", ""))
	{
		SetPopup(std::make_unique<std::function<void()>>([this, data = SimulationPopupData()]() mutable
		{
			NewSimulationPopup(data);
		}));
	}
	if(ImGui::MenuItem("Open", ""))
	{

	}
	if(ImGui::BeginMenu("Open Recent"))
	{
		//TODO
		ImGui::EndMenu();
	}
	GuiHelper::BeginDisabled(!currentSaveFile.has_value());
	if(ImGui::MenuItem("Save", currentSaveFile.has_value() ? currentSaveFile.value().CStr() : ""))
	{
		SaveCurrent();
	}
	GuiHelper::EndDisabled(!currentSaveFile.has_value());
	if(ImGui::MenuItem("Save As..."))
	{
		SetPopup(std::make_unique<std::function<void()>>([this, path = std::array<char, 32>()]() mutable
		{
			NewSaveFilePopup(path);
		}));
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

void Editor::ShowNotification(const Notification& notification)
{
	float textWidth = ImGui::CalcTextSize(notification.message.c_str()).x;
	ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x * 0.5f, window->Size().y * 0.2f }, ImGuiCond_Always, { 0.5f, 0.0f });
	ImGui::SetNextWindowSize({ textWidth + ImGui::GetStyle().WindowPadding.x * 2.0f, -1.0f }, ImGuiCond_Always);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { notification.color.r, notification.color.g, notification.color.b, notification.color.a });
	if(ImGui::Begin("##notification", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped(notification.message.c_str());
		ImGui::End();
	}
	ImGui::PopStyleColor();
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
			SetPopup(nullptr);
		}

		ImGui::EndPopup();
	}
}

void Editor::NewSaveFilePopup(std::array<char, 32>& path)
{
	GuiHelper::CenterNextWindow();
	ImGui::OpenPopup("Save As");
	if(ImGui::BeginPopupModal("Save As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputTextWithHint("##pathInput", "Enter file name...", path.data(), path.size(), ImGuiInputTextFlags_CharsNoBlank);
		std::optional<std::string> error = storage.IsValidFileName(path.data());
		bool exists = storage.FileExists(path.data());
		
		if(error.has_value())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { uiErrorColor.r, uiErrorColor.g, uiErrorColor.b, uiErrorColor.a });
			ImGui::TextWrapped(error.value().c_str());
			ImGui::PopStyleColor();
		}
		else if(exists)
		{
			ImGui::BeginDisabled();
			ImGui::TextWrapped("The file already exists! You can overwrite it...");
			ImGui::EndDisabled();
		}

		ImGui::Spacing();
		int result = GuiHelper::HorizontalButtonSplit(exists ? "Overwrite" : "Save", "Cancel", exists ? std::make_optional(uiErrorColor) : std::nullopt, std::nullopt, error.has_value());
		if(result == 1)
		{
			SaveCurrent(path.data());
		}
		if(result > 0)
		{
			ImGui::CloseCurrentPopup();
			SetPopup(nullptr);
		}

		ImGui::EndPopup();
	}
}
