#pragma once
#include "physics/verletobj.h"

class IConstraint
{
public:
	virtual ~IConstraint() { };
	virtual void Contrain(VerletObj& obj) = 0;
};
