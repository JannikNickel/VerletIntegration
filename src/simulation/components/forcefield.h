#pragma once
#include "ecs/component.h"
#include "structs/vector2.h"
#include "simulation/forcefieldsettings.h"

struct ForceField : Component<ForceField>
{
	ForceFieldSettings settings;
	Vector2 pos;

	ForceField(const ForceFieldSettings& settings, Vector2 pos) : settings(settings), pos(pos) { }
};
