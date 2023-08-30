#include "scene.h"
#include "particleobject.h"
#include "core/rectworld.h"
#include "core/circleworld.h"
#include "utils/nameof.h"
#include "magic_enum.hpp"
#include <algorithm>

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

void Scene::RemoveObject(const std::shared_ptr<SceneObject>& obj)
{
	objects.erase(std::remove_if(objects.begin(), objects.end(), [&target = obj](const std::shared_ptr<SceneObject>& obj)
	{
		return obj == target;
	}));
}

const std::vector<std::shared_ptr<SceneObject>>& Scene::Objects() const
{
	return objects;
}

JsonObj Scene::Serialize() const
{
	JsonObj json = {};
	json[NAMEOF(size)] = size;
	json[NAMEOF(world)] = world.Serialize();
	json[NAMEOF(objects)] = {};
	for(const std::shared_ptr<SceneObject>& obj : objects)
	{
		json[NAMEOF(objects)].push_back(obj->Serialize());
	}
	return json;
}

void Scene::Deserialize(const JsonObj& json)
{
	size = json[NAMEOF(size)];
	world.Deserialize(json[NAMEOF(world)]);

	JsonObj objects = json[NAMEOF(objects)];
	for(size_t i = 0; i < objects.size(); i++)
	{
		JsonObj objJson = objects[i];
		SceneObjectType type = magic_enum::enum_cast<SceneObjectType>(static_cast<std::string>(objJson[ISerializable::typeKey])).value();
		std::shared_ptr<SceneObject> obj = nullptr;
		switch(type)
		{
			case SceneObjectType::Particle:
				obj = std::make_shared<ParticleObject>();
				break;
			case SceneObjectType::Spawner:
				break;
			case SceneObjectType::Link:
				break;
			default:
				throw std::exception("Unknown SceneObjectType!");
		}
		if(obj != nullptr)
		{
			obj->Deserialize(objJson);
			AddObject(obj);
		}
	}
}
