#include "scene.h"
#include "core/rectworld.h"
#include "core/circleworld.h"

std::unique_ptr<World> Scene::CreateWorld(Color background, Color color)
{
	switch(world.shape)
	{
		case WorldShape::Rect:
			return std::make_unique<RectWorld>(background, Vector2::one * size * 0.5f, world.bounds.size, color);
		case WorldShape::Circle:
			return std::make_unique<CircleWorld>(background, Vector2::one * size * 0.5f, world.bounds.radius, color);
		default:
			return nullptr;
	}
}
