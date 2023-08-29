#pragma once
#include "scene.h"
#include "sceneobject.h"
#include "storage.h"
#include "core/world.h"
#include "engine/window.h"
#include "structs/color.h"
#include <memory>
#include <functional>
#include <string>
#include <array>

class Editor
{
	struct Notification
	{
		std::string message;
		Color color;
		float duration;

		Notification(const std::string& message, const Color& color, float duration = 3.0f) : message(message), color(color), duration(duration) { }
	};

public:
	Editor(const std::shared_ptr<Window>& window);
	void Update(double dt);

private:
	std::shared_ptr<Window> window;
	SceneStorage storage = SceneStorage("scenes");

	std::unique_ptr<std::function<void()>> currentPopup = nullptr;
	std::optional<Notification> currentNotification = std::nullopt;
	std::unique_ptr<SceneObject> currentPreview = nullptr;
	std::weak_ptr<SceneObject> currentSelected = std::weak_ptr<SceneObject>();
	std::weak_ptr<SceneObject> currentHovered = std::weak_ptr<SceneObject>();

	std::shared_ptr<Scene> scene = nullptr;
	std::unique_ptr<World> world = nullptr;
	std::optional<FileName> currentSaveFile = std::nullopt;

	void Render(double dt);
	void UI(double dt);
	void Placement(double dt);
	void Selection();
	void SelectionInteraction();

	void SetPopup(std::unique_ptr<std::function<void()>> popupFunc);
	void SetNotification(const std::optional<Notification>& notification);

	void OpenScene(const std::shared_ptr<Scene>& scene, const std::optional<FileName>& file = std::nullopt);
	void SaveCurrent(const std::optional<FileName>& file = std::nullopt);

	void CreatePreview(std::unique_ptr<SceneObject>&& obj);
	void SelectObject(const std::weak_ptr<SceneObject>& obj);

	std::weak_ptr<SceneObject> GetHoveredObject();

	void MainMenuBar();
	void FileMenu();
	void AddMenu();

	void ShowNotification(const Notification& notification);
	void NewSimulationPopup(struct SimulationPopupData& data);
	void NewSaveFilePopup(std::array<char, 32>& path);
};
