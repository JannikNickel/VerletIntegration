#pragma once
#include "ecs/component.h"
#include "ecs/entity.h"

struct Link : Component<Link>
{
	Entity e0 = 0;
	Entity e1 = 0;
	float distance;
	bool restrictMin;
	bool restrictMax;

	Link(float distance, bool restrictMin, bool restrictMax) : distance(distance), restrictMin(restrictMin), restrictMax(restrictMax) { }
};
