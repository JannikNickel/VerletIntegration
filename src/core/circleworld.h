#pragma once
#include "world.h"
#include "physics/constraint.h"
#include "structs/vector2.h"
#include "structs/color.h"

class CircleWorld : public World, public IConstraint
{
public:
	CircleWorld(Color background, Vector2 center, float radius, Color color);
	void Render() override;
	Vector2 Center() override;
	void Contrain(VerletObj& obj) override;

private:
	Vector2 center;
	float radius;
	Color color;
};

