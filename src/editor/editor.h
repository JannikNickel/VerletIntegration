#pragma once
#include "scene.h"
#include "core/world.h"
#include <memory>
#include <functional>

class Editor
{
public:
	void Update(double dt);

private:
	std::unique_ptr<std::function<void()>> currentPopup = nullptr;
	std::shared_ptr<Scene> scene = nullptr;
	std::unique_ptr<World> world = nullptr;

	void Render();
	void OpenScene(std::shared_ptr<Scene> scene);

	void MainMenuBar();
	void FileMenu();
	void AddMenu();

	void NewSimulationPopup(struct SimulationPopupData& data);
};
