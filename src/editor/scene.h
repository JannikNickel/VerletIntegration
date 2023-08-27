#pragma once
#include "sceneobject.h"
#include "structs/vector2.h"
#include "structs/color.h"
#include "core/world.h"
#include <cstdint>
#include <vector>
#include <memory>

enum class WorldShape
{
	Rect,
	Circle
};

struct WorldData
{
	WorldShape shape;
	union
	{
		Vector2 size;
		float radius;
	} bounds;
	Color background;
	Color color;
};

class Scene
{
public:
	Scene(int32_t size, WorldData world) : size(size), world(world), objects({}) { }

	int32_t Size() { return size; }
	std::unique_ptr<World> CreateWorld();
	void AddObject(const std::shared_ptr<SceneObject>& obj);
	void RemoveObject(const std::shared_ptr<SceneObject>& obj);
	const std::vector<std::shared_ptr<SceneObject>>& Objects() const;

private:
	int32_t size;
	WorldData world;
	std::vector<std::shared_ptr<SceneObject>> objects;
};
