#include "archetype.h"

std::unordered_map<ArchetypeId, std::shared_ptr<Archetype>, Archetype::ArchetypeIdHash> Archetype::archetypes = {};
std::unordered_map<ComponentId, std::vector<std::pair<ArchetypeId, size_t>>> Archetype::componentArchetypeMap = {};
