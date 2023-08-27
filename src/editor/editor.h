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
	std::shared_ptr<SceneObject> currentSelected = nullptr;
	std::shared_ptr<SceneObject> currentHovered = nullptr;

	std::shared_ptr<Scene> scene = nullptr;
	std::unique_ptr<World> world = nullptr;

	void Render();
	void UI();
	void Placement();
	void Selection();
	void SelectionInteraction();

	void OpenScene(const std::shared_ptr<Scene>& scene);
	void CreatePreview(std::unique_ptr<SceneObject>&& obj);
	void SelectObject(const std::shared_ptr<SceneObject>& obj);

	std::shared_ptr<SceneObject> GetHoveredObject();

	void MainMenuBar();
	void FileMenu();
	void AddMenu();

	void NewSimulationPopup(struct SimulationPopupData& data);
};
