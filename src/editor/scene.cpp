#include "scene.h"
#include "core/rectworld.h"
#include "core/circleworld.h"

std::unique_ptr<World> Scene::CreateWorld()
{
	switch(world.shape)
	{
		case WorldShape::Rect:
			return std::make_unique<RectWorld>(world.background, Vector2::one * size * 0.5f, world.bounds.size, world.color);
		case WorldShape::Circle:
			return std::make_unique<CircleWorld>(world.background, Vector2::one * size * 0.5f, world.bounds.radius, world.color);
		default:
			return nullptr;
	}
}

void Scene::AddObject(const std::shared_ptr<SceneObject>& obj)
{
	objects.push_back(obj);
}

const std::vector<std::shared_ptr<SceneObject>>& Scene::Objects() const
{
	return objects;
}
