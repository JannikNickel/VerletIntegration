#include "archetype.h"

ComponentId Archetype::nextComponentId = 0;
std::unordered_map<ArchetypeId, Archetype, ArchetypeIdHash> Archetype::archetypes = {};
