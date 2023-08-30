#pragma once
#include "entity.h"
#include "component.h"
#include <vector>
#include <array>
#include <utility>
#include <cassert>

using ArchetypeId = std::vector<ComponentId>;

class Archetype
{
	friend class ArchetypeManager;

	struct ComponentData
	{
		size_t elementSize = 0;
		std::vector<std::uint8_t> data = {};
	};

public:
	Archetype(ArchetypeId id) : id(id) { }

	//comps should be sorted based on componentId in ascending order!
	template<size_t N>
	size_t AddEntity(Entity entity, const std::array<std::tuple<ComponentId, size_t, void*>, N>& comps)
	{
		for(size_t i = 0; i < N; i++)
		{
			ComponentData& compData = components[i];
			assert(compData.elementSize == std::get<1>(comps[i]));
			const uint8_t* begin = reinterpret_cast<const uint8_t*>(std::get<2>(comps[i]));
			compData.data.insert(compData.data.end(), begin, begin + compData.elementSize);
		}
		return size++;
	}

	template<typename T> requires ComponentDerived<T>
	bool HasComponent()
	{
		for(const ComponentId id : this->id)
		{
			if(id == T::componentId)
			{
				return true;
			}
		}
		return false;
	}

	template<typename T> requires ComponentDerived<T>
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

private:
	ArchetypeId id;
	size_t size = 0;
	std::vector<ComponentData> components = {};
};
