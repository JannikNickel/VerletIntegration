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
};

class Scene
{
public:
	Scene(int32_t size, WorldData world) : size(size), world(world), objects({}) { }

	std::unique_ptr<World> CreateWorld(Color background, Color color);

private:
	int32_t size;
	WorldData world;
	std::vector<std::shared_ptr<SceneObject>> objects;
};
