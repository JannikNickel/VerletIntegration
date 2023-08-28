#pragma once
#include "structs/vector2.h"
#include "simulation/components/particle.h"
#include <utility>

class IConstraint
{
public:
	virtual ~IConstraint() { };

	virtual void Contrain(Vector2& pos, Particle& p) const = 0;
	virtual std::pair<Vector2, Vector2> Bounds() const = 0;
};
