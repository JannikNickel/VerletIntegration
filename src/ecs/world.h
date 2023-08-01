#pragma once
#include "entity.h"
#include "component.h"
#include "archetype.h"
#include <vector>
#include <array>
#include <unordered_map>
#include <functional>

template<typename T>
concept ComponentDerived = std::is_base_of<Component<T>, T>::value;

class EcsWorld
{
	struct Record
	{
		Archetype& archetype;
		size_t index;
	};

public:
	template<typename... Components> requires (ComponentDerived<Components>&&...)
	Entity CreateEntity(Components&&... components)
	{
		Entity e = ++nextEntity;

		constexpr size_t numComps = sizeof...(Components);
		std::array<ComponentBase*, numComps> comps = { &components... };
		//std::array<size_t, numComps> compSizes = { sizeof(Components)... };

		std::ranges::sort(comps, [](const ComponentBase* lhs, const ComponentBase* rhs) { return lhs->CompId() < rhs->CompId(); });
		Archetype& archetype = Archetype::FromComponents(comps);
		size_t index = archetype.AddEntity(e, comps);
		entities.insert(std::make_pair(e, Record { archetype, index }));

		return e;
	}

	template<typename T> requires ComponentDerived<T>
	bool HasComponent(Entity entity)
	{
		Archetype& archetype = entities.at(entity).archetype;
		return archetype.HasComponent(T::componentId);
	}

	template<typename T> requires ComponentDerived<T>
	T* GetComponent(Entity entity)
	{
		const Record& record = entities.at(entity);
		Archetype& archetype = record.archetype;
		return archetype.GetComponent<T>(record.index);
	}

private:
	uint32_t nextEntity = 0;
	std::unordered_map<Entity, Record> entities = {};
};
