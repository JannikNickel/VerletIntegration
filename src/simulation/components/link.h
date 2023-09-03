#pragma once
#include "ecs/component.h"

struct Link : Component<Link>
{
	float distance;
	bool restrictMin;
	bool restrictMax;

	Link(float distance, bool restrictMin, bool restrictMax) : distance(distance), restrictMin(restrictMin), restrictMax(restrictMax) { }
};
