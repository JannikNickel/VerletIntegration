#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>

using ComponentId = uint32_t;
using ArchetypeId = std::vector<ComponentId>;

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

class Archetype
{
public:
	Archetype() : id({}) { }

	template<class T>
	static ComponentId RegisterComponent()
	{
		return ++nextComponentId;
	}

	static Archetype& FromComponents(std::vector<ComponentId> ids)
	{
		auto it = archetypes.find(ids);
		if(it != archetypes.end())
		{
			return it->second;
		}

		Archetype archetype = Archetype(ids);
		archetypes[ids] = archetype;
		return archetype;
	}

private:
	ArchetypeId id;

	Archetype(ArchetypeId id) : id(id) { }

	static ComponentId nextComponentId;
	static std::unordered_map<ArchetypeId, Archetype, ArchetypeIdHash> archetypes;
};
