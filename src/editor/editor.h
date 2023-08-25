#pragma once
#include "scene.h"
#include "sceneobject.h"
#include "core/world.h"
#include <memory>
#include <functional>

class Editor
{
public:
	void Update(double dt);

private:
	std::unique_ptr<std::function<void()>> currentPopup = nullptr;
	std::unique_ptr<SceneObject> currentPreview = nullptr;

	std::shared_ptr<Scene> scene = nullptr;
	std::unique_ptr<World> world = nullptr;

	void Render();
	void UI();
	void Control();

	void OpenScene(std::shared_ptr<Scene> scene);
	void CreatePreview(std::unique_ptr<SceneObject>&& obj);

	void MainMenuBar();
	void FileMenu();
	void AddMenu();

	void NewSimulationPopup(struct SimulationPopupData& data);
};
