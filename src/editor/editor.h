#pragma once
#include "scene.h"
#include "sceneobject.h"
#include "core/world.h"
#include "engine/window.h"
#include <memory>
#include <functional>

class Editor
{
public:
	Editor(const std::shared_ptr<Window>& window);
	void Update(double dt);

private:
	std::shared_ptr<Window> window;

	std::unique_ptr<std::function<void()>> currentPopup = nullptr;
	std::unique_ptr<SceneObject> currentPreview = nullptr;
	std::weak_ptr<SceneObject> currentSelected = std::weak_ptr<SceneObject>();
	std::weak_ptr<SceneObject> currentHovered = std::weak_ptr<SceneObject>();

	std::shared_ptr<Scene> scene = nullptr;
	std::unique_ptr<World> world = nullptr;

	void Render(double dt);
	void UI();
	void Placement(double dt);
	void Selection();
	void SelectionInteraction();

	void OpenScene(const std::shared_ptr<Scene>& scene);
	void CreatePreview(std::unique_ptr<SceneObject>&& obj);
	void SelectObject(const std::weak_ptr<SceneObject>& obj);

	std::weak_ptr<SceneObject> GetHoveredObject();

	void MainMenuBar();
	void FileMenu();
	void AddMenu();

	void NewSimulationPopup(struct SimulationPopupData& data);
};
