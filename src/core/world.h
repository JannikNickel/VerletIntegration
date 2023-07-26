#pragma once
#include "physics/constraint.h"
#include "structs/color.h"
#include "structs/vector2.h"

class World
{
public:
	World(Color backgroundColor);

	virtual ~World();
	virtual void Render();
	virtual Vector2 Center() = 0;

protected:
	Color background;
};
