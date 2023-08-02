#pragma once
#include "entity.h"
#include "component.h"
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>
#include <ranges>
#include <memory>
#include <functional>
#include <cassert>

using ArchetypeId = std::vector<ComponentId>;

class Archetype
{
	struct ArchetypeIdHash
	{
		size_t operator()(const ArchetypeId& id) const
		{
			std::hash<int> hasher = {};
			size_t seed = 277;
			for(const ComponentId i : id)
			{
				seed = seed * 47 + hasher(i);
			}
			return seed;
		}
	};

	struct ComponentData
	{
		size_t elementSize = 0;
		std::vector<std::uint8_t> data = {};
	};

public:
	Archetype() : id({}) { }
	Archetype(ArchetypeId id) : id(id) { }

	//comps should be sorted based on CompId in ascending order!
	template<size_t N>
	size_t AddEntity(Entity entity, const std::array<ComponentBase*, N>& comps)
	{
		for(size_t i = 0; i < N; i++)
		{
			ComponentData& compData = components[i];
			assert(compData.elementSize == comps[i]->Size());
			const uint8_t* begin = reinterpret_cast<const uint8_t*>(comps[i]);
			compData.data.insert(compData.data.end(), begin, begin + compData.elementSize);
		}
		return size++;
	}

	bool HasComponent(ComponentId compId)
	{
		for(const ComponentId id : this->id)
		{
			if(id == compId)
			{
				return true;
			}
		}
		return false;
	}

	template<typename T>
	T* GetComponent(size_t index)
	{
		ComponentId compId = T::componentId;
		for(size_t i = 0; i < id.size(); i++)
		{
			if(id[i] == compId)
			{
				return reinterpret_cast<T*>(&components[i].data.data()[index * components[i].elementSize]);
			}
		}
	}

	//comps should be sorted based on CompId in ascending order!
	template<size_t N>
	static std::shared_ptr<Archetype> FromComponents(const std::array<ComponentBase*, N>& comps)
	{
		auto idView = std::views::all(comps) | std::views::transform([](const ComponentBase* comp) { return comp->CompId(); });
		ArchetypeId id = ArchetypeId(idView.begin(), idView.end());

		auto it = archetypes.find(id);
		if(it != archetypes.end())
		{
			return it->second;
		}

		std::shared_ptr<Archetype> archetype = std::make_shared<Archetype>(id);
		for(size_t i = 0; i < N; i++)
		{
			const ComponentBase* comp = comps[i];
			archetype->components.push_back(ComponentData { comp->Size(), {} });

			auto [cIt, _] = componentArchetypeMap.emplace(comp->CompId(), std::vector<std::pair<ArchetypeId, size_t>>());
			cIt->second.push_back(std::make_pair(id, i));
		}
		archetypes.insert(std::make_pair(id, archetype));
		return archetype;
	}

	template<typename... Components, size_t... Indices>
	static std::tuple<Components*...> UnwrapCompPtr(ComponentData** compData, std::index_sequence<Indices...>)
	{
		return std::make_tuple(reinterpret_cast<Components*>(&compData[Indices]->data[0])...);
	}

	template<typename... Components, size_t... Indices>
	static void InvokeEntityFunc(std::function<void(Components&...)>& entityFunc, std::tuple<Components*...>& comps, std::index_sequence<Indices...>)
	{
		entityFunc(*std::get<Indices>(comps)...);
	}

	template<typename... Components>
	static void QueryComponents(std::function<void(Components&...)> entityFunc)
	{
		constexpr size_t numComps = sizeof...(Components);
		std::array<ComponentId, numComps> comps = { Components::componentId... };

		std::vector<std::pair<ArchetypeId, std::array<size_t, numComps>>> archs = {};
		for(const std::pair<ArchetypeId, size_t>& pair : componentArchetypeMap[comps[0]])
		{
			std::array<size_t, numComps> arr = { pair.second };
			archs.push_back(std::make_pair(pair.first, arr));
		}
		for(size_t i = 1; i < numComps; i++)
		{
			const std::vector<std::pair<ArchetypeId, size_t>>& cArchs = componentArchetypeMap[comps[i]];
			for(int k = archs.size() - 1; k >= 0; k--)
			{
				std::pair<ArchetypeId, std::array<size_t, numComps>>& arch = archs[k];
				auto it = std::ranges::find_if(cArchs, [&](const std::pair<ArchetypeId, size_t>& pair) { return pair.first == arch.first; });
				if(it == cArchs.end())
				{
					archs.erase(archs.begin() + k);
					continue;
				}
				arch.second[i] = it->second;
			}
		}

		for(auto& [id, indices] : archs)
		{
			std::shared_ptr<Archetype> archetype = Archetype::archetypes[id];
			std::array<ComponentData*, numComps> compData = {};
			for(size_t i = 0; i < numComps; i++)
			{
				ComponentData* data = &archetype->components[indices[i]];
				compData[i] = data;
			}

			std::tuple<Components*...> comps = UnwrapCompPtr<Components...>(compData.data(), std::make_index_sequence<numComps>{});
			size_t compCount = compData[0]->data.size() / compData[0]->elementSize;
			for(size_t i = 0; i < compCount; i++)
			{
				InvokeEntityFunc(entityFunc, comps, std::make_index_sequence<numComps>{});
				((std::get<Components*>(comps)++), ...);
			}
		}
	}

private:
	ArchetypeId id;
	size_t size = 0;
	std::vector<ComponentData> components = {};
	//Could store another vector of entity ids to know them too

	static std::unordered_map<ArchetypeId, std::shared_ptr<Archetype>, ArchetypeIdHash> archetypes;
	static std::unordered_map<ComponentId, std::vector<std::pair<ArchetypeId, size_t>>> componentArchetypeMap;
};
