#pragma once
#include "physics/verletobj.h"
#include "simulation/components/physicscircle.h"

class IConstraint
{
public:
	virtual ~IConstraint() { };
	virtual void Contrain(Vector2& pos, const PhysicsCircle& p) = 0;
};
