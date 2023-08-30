#pragma once
#include "physics/constraint.h"
#include "structs/color.h"
#include "structs/vector2.h"

class World : public IConstraint
{
public:
	World(Color backgroundColor);

	virtual ~World();
	virtual void Render();
	virtual Vector2 Center() const = 0;
	virtual bool Contains(Vector2 point) const = 0;

protected:
	Color background;
};
