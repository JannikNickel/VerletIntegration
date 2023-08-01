#pragma once
#include "entity.h"
#include "component.h"
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>
#include <ranges>
#include <assert.h>

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

	template<class T>
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
	static Archetype& FromComponents(const std::array<ComponentBase*, N>& comps)
	{
		auto idView = std::views::all(comps) | std::views::transform([](const ComponentBase* comp) { return comp->CompId(); });
		ArchetypeId id = ArchetypeId(idView.begin(), idView.end());

		auto it = archetypes.find(id);
		if(it != archetypes.end())
		{
			return it->second;
		}

		Archetype archetype = Archetype(id);
		for(const ComponentBase* comp : comps)
		{
			archetype.components.push_back(ComponentData { comp->Size(), {} });
		}
		auto [eIt, _] = archetypes.emplace(id, std::move(archetype));
		return eIt->second;
	}

private:
	ArchetypeId id;
	size_t size = 0;
	std::vector<ComponentData> components = {};
	//Could store another vector of entity ids to know them too

	Archetype(ArchetypeId id) : id(id) { }

	static std::unordered_map<ArchetypeId, Archetype, ArchetypeIdHash> archetypes;
};
