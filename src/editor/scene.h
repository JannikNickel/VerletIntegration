#pragma once
#include "serialization/serializable.h"
#include "sceneobject.h"
#include "worlddata.h"
#include "physicsdata.h"
#include "core/world.h"
#include "simulation/simulation.h"
#include <cstdint>
#include <vector>
#include <memory>

class Scene : public ISerializable
{
	friend class Editor;

public:
	Scene(int32_t size, WorldData world) : size(size), world(world), physics(PhysicsData()), objects({}) { }
	Scene() : Scene(0, WorldData()) { }

	int32_t Size() const { return size; }
	const std::vector<std::shared_ptr<SceneObject>>& Objects() const { return objects; }

	std::unique_ptr<World> CreateWorld() const;
	Simulation CreateSimulation() const;

	void AddObject(const std::shared_ptr<SceneObject>& obj, bool assignId = true);
	void RemoveObject(std::shared_ptr<SceneObject>&& obj);
	std::weak_ptr<SceneObject> FindObject(uint32_t id) const;

	JsonObj Serialize() const override;
	void Deserialize(const JsonObj& json) override;

private:
	int32_t size;
	WorldData world;
	PhysicsData physics;
	std::vector<std::shared_ptr<SceneObject>> objects;

	uint32_t lastId = 0;
};
