#include "archetype.h"

std::unordered_map<ArchetypeId, Archetype, Archetype::ArchetypeIdHash> Archetype::archetypes = {};
