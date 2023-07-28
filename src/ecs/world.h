#pragma once
#include "entity.h"
#include "component.h"
#include "archetype.h"
#include <vector>
#include <unordered_map>
#include <functional>

template <typename T>
concept ComponentBaseDerived = std::is_base_of<ComponentBase, T>::value;

class EcsWorld
{
public:
	template<typename... Components> requires (ComponentBaseDerived<Components>&&...)
	Entity CreateEntity(Components&&... components)
	{
		Entity e = ++nextEntity;

		constexpr size_t numComps = sizeof...(Components);
		std::tuple<Components...> comps = std::make_tuple(std::forward<Components>(components)...);

		std::vector<ComponentId> ids = {};
		ids.reserve(numComps);
		ForEachComponent(comps, [&](const ComponentBase* comp)
		{
			ids.push_back(comp->CompId());
		});

		Archetype& archetype = Archetype::FromComponents(ids);
		entities.insert(std::make_pair(e, std::ref(archetype)));

		return e;
	}

private:
	uint32_t nextEntity = 0;
	std::unordered_map<Entity, std::reference_wrapper<Archetype>> entities = {};

	template <typename... Components, size_t... ISeq, typename Function>
	void ForEachComponentImpl(const std::tuple<Components...>& comps, Function&& func, std::index_sequence<ISeq...>)
	{
		(func(static_cast<const ComponentBase*>(&std::get<ISeq>(comps))), ...);
	}

	template <typename... Components, typename Function>
	void ForEachComponent(const std::tuple<Components...>& comps, Function&& func)
	{
		ForEachComponentImpl(comps, std::forward<Function>(func), std::index_sequence_for<Components...>{});
	}
};
