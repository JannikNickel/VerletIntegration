#pragma once
#include "entity.h"
#include "component.h"
#include "archetypemanager.h"
#include "archetype.h"
#include "threadpool.h"
#include <cstdint>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <optional>
#include <span>

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
		std::array<std::tuple<ComponentId, size_t, void*>, numComps> comps = { std::make_tuple(Components::componentId, sizeof(std::decay_t<Components>), &components)... };

		std::ranges::sort(comps, [](const std::tuple<ComponentId, size_t, void*>& lhs, const std::tuple<ComponentId, size_t, void*>& rhs) { return std::get<0>(lhs) < std::get<0>(rhs); });
		std::shared_ptr<Archetype> archetype = archManager.ArchetypeFromComponents(comps);
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

	template<typename... Components, typename Func>
		requires (ComponentDerived<Components>&&...) && (sizeof...(Components) > 0) && std::is_invocable_r_v<void, Func, Components&...>
	void Query(Func&& entityFunc)
	{
		archManager.QueryComponents<Func, Components...>(std::forward<Func>(entityFunc));
	}

	template<typename... Components, typename Func>
		requires (ComponentDerived<Components>&&...) && (sizeof...(Components) > 0) && std::is_invocable_r_v<void, Func, Components&...>
	void QueryMT(const std::optional<uint32_t>& threads, Func&& entityFunc)
	{
		archManager.QueryComponentsMT<Func, Components...>(std::forward<Func>(entityFunc), threadPool, threads);
	}

	template<typename... Components, typename Func>
		requires (ComponentDerived<Components>&&...) && (sizeof...(Components) > 0) && std::is_invocable_r_v<void, Func, Components*..., size_t>
	void QueryChunked(size_t chunkSize, Func&& entityFunc)
	{
		archManager.QueryComponentsChunked<Func, Components...>(std::forward<Func>(entityFunc), chunkSize);
	}

	template<typename... Components, typename Func>
		requires (ComponentDerived<Components>&&...) && (sizeof...(Components) > 0) && std::is_invocable_r_v<void, Func, Components&..., Components&...>
	void QueryPairs(Func&& entityFunc)
	{
		archManager.QueryComponentPairs<Func, Components...>(std::forward<Func>(entityFunc));
	}

	template<typename Component, typename Func>
		requires ComponentDerived<Component>&& std::is_invocable_v<Func, std::span<const Component>>
	void WithAllOfComponent(Func&& componentFunc)
	{
		bool fallback = true;
		QueryChunked<Component>(std::numeric_limits<size_t>::max(), [&](Component* components, size_t size)
		{
			fallback = false;
			componentFunc(std::span<const Component>(components, size));
		});
		if(fallback)
		{
			componentFunc(std::span<const Component>{});
		}
	}

private:
	uint32_t nextEntity = 0;
	std::unordered_map<Entity, Record> entities = {};
	ThreadPool threadPool = {};
	ArchetypeManager archManager = {};
};
