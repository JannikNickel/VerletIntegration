#include "scene.h"
#include "particleobject.h"
#include "spawnerobject.h"
#include "linkobject.h"
#include "core/rectworld.h"
#include "core/circleworld.h"
#include "utils/nameof.h"
#include "connectionplacement.h"
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

Simulation Scene::CreateSimulation()
{
	SolverSettings settings = this->physics.Settings();
	settings.partitioningSize = ParticleObject::maxSize * 2.0f;
	Simulation sim = Simulation(CreateWorld(), settings);
	for(const std::shared_ptr<SceneObject>& obj : objects)
	{
		switch(obj->ObjType())
		{
			case SceneObjectType::Particle:
			{
				ParticleObject& p = static_cast<ParticleObject&>(*obj);
				sim.AddParticle(Particle(p.radius, p.mass, p.bounciness, p.pinned, obj->position, Vector2::zero), obj->position, p.color, obj->id);
				break;
			}
			case SceneObjectType::Spawner:
			{
				SpawnerObject& s = static_cast<SpawnerObject&>(*obj);
				sim.AddSpawner(Spawner(obj->position, s.Settings()), obj->id);
				break;
			}
			case SceneObjectType::Link:
			{
				LinkObject& l = static_cast<LinkObject&>(*obj);
				sim.AddLink(Link(0.0f, l.RestrictMin(), l.RestrictMax()), l[0].lock()->Id(), l[1].lock()->Id(), l.Col(), obj->id);
				break;
			}
			default:
				continue;
		}
	}
	return sim;
}

void Scene::AddObject(const std::shared_ptr<SceneObject>& obj, bool assignId)
{
	if(assignId)
	{
		obj->id = ++lastId;
	}
	bool first = dynamic_cast<IConnectionPlacement*>(obj.get()) != nullptr;
	objects.insert(first ? objects.begin() : objects.end(), obj);
}

void Scene::RemoveObject(std::shared_ptr<SceneObject>&& obj)
{
	objects.erase(std::remove_if(objects.begin(), objects.end(), [target = std::move(obj)](const std::shared_ptr<SceneObject>& obj)
	{
		return obj == target;
	}));

	for(int i = objects.size() - 1; i >= 0; i--)
	{
		if(!objects[i]->IsValid())
		{
			objects.erase(objects.begin() + i);
		}
	}
}

std::weak_ptr<SceneObject> Scene::FindObject(uint32_t id) const
{
	auto it = std::find_if(objects.begin(), objects.end(), [id](const std::shared_ptr<SceneObject>& obj) { return obj->Id() == id; });
	if(it != objects.end())
	{
		return *it;
	}
	return std::weak_ptr<SceneObject>();
}

JsonObj Scene::Serialize() const
{
	JsonObj json = {};
	json[NAMEOF(size)] = size;
	json[NAMEOF(world)] = world.Serialize();
	json[NAMEOF(physics)] = physics.Serialize();
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
	physics.Deserialize(json[NAMEOF(physics)]);

	JsonObj objects = json[NAMEOF(objects)];
	std::sort(objects.begin(), objects.end(), [](const JsonObj& a, const JsonObj& b)
	{
		return static_cast<uint32_t>(a[NAMEOF(SceneObject::id)]) < static_cast<uint32_t>(b[NAMEOF(SceneObject::id)]);
	});
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
				obj = std::make_shared<SpawnerObject>();
				break;
			case SceneObjectType::Link:
				obj = std::make_shared<LinkObject>(*this);
				break;
			default:
				throw std::exception("Unknown SceneObjectType!");
		}
		if(obj != nullptr)
		{
			obj->Deserialize(objJson);
			lastId = std::max(lastId, obj->id);
			AddObject(obj, false);
		}
	}
}
