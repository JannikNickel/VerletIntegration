#pragma once
#include "serialization/serializable.h"
#include "sceneobject.h"
#include "worlddata.h"
#include "core/world.h"
#include <cstdint>
#include <vector>
#include <memory>

class Scene : public ISerializable
{
public:
	Scene(int32_t size, WorldData world) : size(size), world(world), objects({}) { }

	int32_t Size() { return size; }
	std::unique_ptr<World> CreateWorld();
	void AddObject(const std::shared_ptr<SceneObject>& obj);
	void RemoveObject(const std::shared_ptr<SceneObject>& obj);
	const std::vector<std::shared_ptr<SceneObject>>& Objects() const;

	JsonObj Serialize() const override;
	void Deserialize() override;

private:
	int32_t size;
	WorldData world;
	std::vector<std::shared_ptr<SceneObject>> objects;
};
