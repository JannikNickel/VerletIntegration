#pragma once
#include "ecs/component.h"
#include "structs/color.h"

struct RenderColor : Component<RenderColor>
{
	Color value;

	RenderColor(Color value) : value(value) { }
};
