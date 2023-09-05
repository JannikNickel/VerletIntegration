#include "editor.h"
#include "editorsettings.h"
#include "guihelper.h"
#include "particleobject.h"
#include "spawnerobject.h"
#include "linkobject.h"
#include "serialization/serializable.h"
#include "renderer/graphics.h"
#include "engine/input.h"
#include "imgui.h"
#include "magic_enum.hpp"
#include <algorithm>
#include <cstdint>
#include <format>

using namespace EditorSettings;

struct SimulationPopupData
{
	int32_t sceneSize = maxSceneSize;
	WorldData worldData = {	WorldShape::Rect, { Vector2(sceneSize, sceneSize) }, Color::From32(30, 30, 30), Color::From32(15, 15, 15) };
};

struct LoadFilePopupData
{
	std::array<char, 32> path = { 0 };
	std::optional<FileName> selected = std::nullopt;
};

Editor::Editor(const std::shared_ptr<Window>& window, const std::function<void(Simulation)>& simulationCallback) : window(window), simulationCallback(simulationCallback)
{

}

void Editor::Update(double dt)
{
	static bool init = [this]()
	{
		EditorSettings::ApplyUIStyle();
		OpenScene(std::make_unique<Scene>(maxSceneSize, WorldData { WorldShape::Circle, {.radius = 500.0f }, Color::From32(30, 30, 30), Color::From32(15, 15, 15) }));
		return true;
	}();

	Render(dt);
	UI(dt);
	Selection();
	SelectionInteraction();
	Placement(dt);
	Insertion();
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
		bool lmb = Input::KeyPressed(KeyCode::LMB);

		std::weak_ptr<SceneObject> hovered = GetHoveredObject();
		IConnectionPlacement* cp = dynamic_cast<IConnectionPlacement*>(currentPreview.get());
		bool valid = world->Contains(pos) && hovered.expired() != (cp != nullptr);
		bool previewValid = valid;
		if(cp != nullptr)
		{
			bool pDiff = (*cp)[0].expired() || (*cp)[0].lock().get() != hovered.lock().get();
			bool pTarget = dynamic_cast<IConnectionPlacement*>(hovered.lock().get()) == nullptr;
			if(valid && pDiff && pTarget && lmb)
			{
				((*cp)[0].expired() ? (*cp)[0] : (*cp)[1]) = hovered;
			}
			valid &= !(*cp)[0].expired() && !(*cp)[1].expired() && pDiff && pTarget;
			previewValid &= !(*cp)[0].expired() && pDiff;
		}

		currentPreview->position = pos;
		currentPreview->Render(dt, previewValid ? previewValidColor : previewInvalidColor);

		if(valid && lmb)
		{
			std::unique_ptr<SceneObject> next = currentPreview->Clone();
			scene->AddObject(std::move(currentPreview));
			currentPreview = std::move(next);
		}
		else if(Input::KeyPressed(KeyCode::Escape))
		{
			currentPreview = nullptr;
		}
	}
}

void Editor::Selection()
{
	currentHovered = GetHoveredObject();

	if(currentPreview != nullptr)
	{
		if(dynamic_cast<IConnectionPlacement*>(currentPreview.get()) == nullptr)
		{
			currentHovered.reset();
		}
		return;
	}

	if(Input::KeyPressed(KeyCode::LMB) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByPopup) && !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopup))
	{
		SelectObject(currentHovered);
	}
}

void Editor::SelectionInteraction()
{
	static std::weak_ptr<SceneObject> lastSelected = {};
	if(std::shared_ptr<SceneObject> selected = currentSelected.lock())
	{
		bool selectionChanged = lastSelected.lock() != selected;
		lastSelected = selected;

		Vector2 pos = selected->position;
		float xDir = pos.x / static_cast<float>(scene->Size()) > 0.75f ? -1.0f : 1.0f;
		ImGui::SetNextWindowPos({ pos.x + xDir * 25.0f, window->Size().y - pos.y - GuiHelper::TitleBarHeight() * 0.5f }, selectionChanged ? ImGuiCond_Always : ImGuiCond_Appearing, { (-xDir + 1.0f) * 0.5f, 0.0f });
		EditResult result = EditResult::None;
		std::string title = std::string(magic_enum::enum_name(selected->ObjType()));
		if(ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			result = selected->Edit();
			ImGui::End();
		}

		if(result == EditResult::Delete || Input::KeyPressed(KeyCode::Delete))
		{
			scene->RemoveObject(std::move(selected));
		}
		else if(result == EditResult::Duplicate)
		{
			CreatePreview(selected->Clone());
		}
		else if(Input::KeyHeld(KeyCode::LeftControl) && Input::KeyPressed(KeyCode::C) && dynamic_cast<IConnectionPlacement*>(selected.get()) == nullptr)
		{
			currentCopied = selected->Clone();
		}
		else if(Input::KeyPressed(KeyCode::Escape))
		{
			SelectObject({});
		}
	}
}

void Editor::Insertion()
{
	if(currentCopied != nullptr)
	{
		if(Input::KeyHeld(KeyCode::LeftControl) && Input::KeyPressed(KeyCode::V))
		{
			CreatePreview(currentCopied->Clone());
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

void Editor::OpenScene(std::unique_ptr<Scene> scene, const std::optional<FileName>& file)
{
	Graphics::SetProjection(scene->Size(), scene->Size());
	this->world = scene->CreateWorld();
	this->scene = std::move(scene);
	this->currentSaveFile = file;

	SetNotification(Notification(file.has_value() ? std::format("Opened scene \"{0}\"!", file.value().Str()) : "Opened scene!", uiSuccessColor, 1.0f));
}

void Editor::LoadScene(const FileName& file)
{
	std::optional<JsonObj> json = storage.LoadFile(file);
	if(!json.has_value())
	{
		SetNotification(Notification("Could not parse file!", uiErrorColor));
		return;
	}

	std::unique_ptr<Scene> scene = std::make_unique<Scene>();
	try
	{
		scene->Deserialize(json.value());
	} 
	catch(const std::exception& e)
	{
		SetNotification(Notification(std::format("Could not deserialize file! ({0})", e.what()), uiErrorColor));
		return;
	}

	OpenScene(std::move(scene), file);
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

void Editor::CreatePreview(std::unique_ptr<SceneObject> obj)
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
		if(ImGui::BeginMenu("Physics"))
		{
			scene->physics.Edit();
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Controls"))
		{
			ControlsMenu();
			ImGui::EndMenu();
		}

		bool allowSimulation = scene != nullptr && currentPreview == nullptr && currentSelected.expired();
		bool simShortcut = allowSimulation && Input::KeyPressed(KeyCode::Enter) && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
		if(ImGui::MenuItem("Simulate", nullptr, nullptr, allowSimulation) || simShortcut)
		{
			simulationCallback(scene->CreateSimulation());
		}

		std::string controlInfo = "";
		if(currentPreview != nullptr)
		{
			controlInfo = std::format("Placing {0}... (esc to cancel)", magic_enum::enum_name(currentPreview->ObjType()));
		}
		else if(std::shared_ptr<SceneObject> selected = currentSelected.lock())
		{
			controlInfo = std::format("Selected {0}... (esc to deselect, del to delete)", magic_enum::enum_name(selected->ObjType()));
		}
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(controlInfo.c_str()).x);
		ImGui::Text(controlInfo.c_str());

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
		SetPopup(std::make_unique<std::function<void()>>([this, data = LoadFilePopupData()]() mutable
		{
			LoadFilePopup(data);
		}));
	}
	if(ImGui::BeginMenu("Open Recent"))
	{
		for(const FileName& file : storage.RecentFiles())
		{
			if(ImGui::MenuItem(file.CStr()))
			{
				LoadScene(file);
				break;
			}
		}
		ImGui::EndMenu();
	}
	ImGui::BeginDisabled(!currentSaveFile.has_value());
	if(ImGui::MenuItem("Save", currentSaveFile.has_value() ? currentSaveFile.value().CStr() : ""))
	{
		SaveCurrent();
	}
	ImGui::EndDisabled();
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
		CreatePreview(std::make_unique<ParticleObject>());
	}
	if(ImGui::MenuItem("Particle Spawner", ""))
	{
		CreatePreview(std::make_unique<SpawnerObject>());
	}
	if(ImGui::MenuItem("Link", ""))
	{
		CreatePreview(std::make_unique<LinkObject>(*scene));
	}
}

void Editor::ControlsMenu()
{
	const char* text =
		"Editor\n"
		"- LMB = Place preview\n"
		"- Esc = Cancel placement/selection\n"
		"- Del = Delete selected\n"
		"\nSimulation\n"
		"- Enter = Start\n"
		"- Esc   = Stop\n"
		"- Space = Toggle stats";
	ImGui::Spacing();
	ImGui::Text(text);
}

void Editor::ShowNotification(const Notification& notification)
{
	float textWidth = ImGui::CalcTextSize(notification.message.c_str()).x;
	ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x * 0.5f, window->Size().y * 0.2f }, ImGuiCond_Always, { 0.5f, 0.0f });
	ImGui::SetNextWindowSize({ textWidth + ImGui::GetStyle().WindowPadding.x * 2.0f, -1.0f }, ImGuiCond_Always);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { notification.color.r, notification.color.g, notification.color.b, 0.5f });
	if(ImGui::Begin("##notification", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped(notification.message.c_str());
		ImGui::End();
	}
	ImGui::PopStyleColor();
}

void Editor::NewSimulationPopup(SimulationPopupData& data)
{
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
					data.worldData.bounds.size.x = std::clamp(data.worldData.bounds.size.x, minSceneSize * 0.5f, static_cast<float>(data.sceneSize));
					data.worldData.bounds.size.y = std::clamp(data.worldData.bounds.size.y, minSceneSize * 0.5f, static_cast<float>(data.sceneSize));
				}
				break;
			}
			case WorldShape::Circle:
			{
				ImGui::LabelText("", "World Radius");
				if(ImGui::InputFloat("##worldSizeInput", &data.worldData.bounds.radius, 0.0f, 0.0f, "%.0f") || clamp)
				{
					data.worldData.bounds.radius = std::clamp(data.worldData.bounds.radius, minSceneSize * 0.25f, static_cast<float>(data.sceneSize * 0.5f));
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
			OpenScene(std::make_unique<Scene>(data.sceneSize, data.worldData));
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
		ImGui::InputTextWithHint("##pathInput", "Enter file name...", path.data(), path.size() - 1, ImGuiInputTextFlags_CharsNoBlank);
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

void Editor::LoadFilePopup(LoadFilePopupData& data)
{
	GuiHelper::CenterNextWindow();
	ImGui::OpenPopup("Load");
	if(ImGui::BeginPopupModal("Load", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputTextWithHint("##pathInput", "Search file...", data.path.data(), data.path.size() - 1, ImGuiInputTextFlags_CharsNoBlank);
		ImGui::Spacing();

		std::string filter = std::string(data.path.data());
		std::optional<FileName> fileToDelete = std::nullopt;
		if(ImGui::BeginChild("Files", { ImGui::GetContentRegionAvail().x, 50.0f }))
		{
			int32_t amount = 0;
			storage.ForEach([&](FileName file)
			{
				if(file.Str().find(filter) != std::string::npos)
				{
					bool selected = data.selected == file;
					if(ImGui::Selectable(file.CStr(), &selected))
					{
						data.selected = selected ? std::make_optional(file) : std::nullopt;
					}
					if(ImGui::BeginPopupContextItem(file.CStr()))
					{
						if(ImGui::MenuItem("Delete"))
						{
							fileToDelete = file;
						}
						ImGui::EndPopup();
					}
					amount++;
				}
			});
			if(amount == 0)
			{
				ImGui::BeginDisabled();
				ImGui::TextWrapped("No files match the search...");
				ImGui::EndDisabled();
			}
			ImGui::EndChild();
		}
		if(fileToDelete.has_value())
		{
			storage.DeleteFile(fileToDelete.value());
		}

		ImGui::Spacing();
		int result = GuiHelper::HorizontalButtonSplit("Load", "Cancel", std::nullopt, std::nullopt, !data.selected.has_value());
		if(result == 1)
		{
			LoadScene(data.selected.value());
		}
		if(result > 0)
		{
			ImGui::CloseCurrentPopup();
			SetPopup(nullptr);
		}

		ImGui::EndPopup();
	}
}
