#pragma once
#include "entity.h"
#include "component.h"
#include "archetype.h"
#include <cstdint>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>

class EcsWorld
{
	struct Record
	{
		std::shared_ptr<Archetype> archetype;
		size_t index;
	};

public:
	template<typename... Components> requires (ComponentDerived<Components>&&...)
	Entity CreateEntity(Components&&... components)
	{
		Entity e = ++nextEntity;

		constexpr size_t numComps = sizeof...(Components);
		std::array<ComponentBase*, numComps> comps = { &components... };

		std::ranges::sort(comps, [](const ComponentBase* lhs, const ComponentBase* rhs) { return lhs->CompId() < rhs->CompId(); });
		std::shared_ptr<Archetype> archetype = Archetype::FromComponents(comps);
		size_t index = archetype->AddEntity(e, comps);
		entities.insert(std::make_pair(e, Record { archetype, index }));

		return e;
	}

	template<typename T> requires ComponentDerived<T>
	bool HasComponent(Entity entity)
	{
		const std::shared_ptr<Archetype>& archetype = entities.at(entity).archetype;
		return archetype->HasComponent<T>();
	}

	template<typename T> requires ComponentDerived<T>
	T* GetComponent(Entity entity)
	{
		const Record& record = entities.at(entity);
		return record.archetype->GetComponent<T>(record.index);
	}

	template<typename... Components> requires (ComponentDerived<Components>&&...)
	void Query(std::function<void(Components&...)> entityFunc)
	{
		Archetype::QueryComponents(entityFunc);
	}

	template<typename... Components> requires (ComponentDerived<Components>&&...)
	void QueryChunked(size_t chunkSize, std::function<void(Components*..., size_t amount)> entityFunc)
	{
		Archetype::QueryComponentsChunked<Components...>(entityFunc, chunkSize);
	}

private:
	uint32_t nextEntity = 0;
	std::unordered_map<Entity, Record> entities = {};
};
