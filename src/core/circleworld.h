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
	Vector2 Center() const override;
	void Contrain(Vector2& pos, const Particle& p) const override;
	std::pair<Vector2, Vector2> Bounds() const override;

private:
	Vector2 center;
	float radius;
	Color color;
};
