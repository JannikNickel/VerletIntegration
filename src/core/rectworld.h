#pragma once
#include "world.h"
#include "physics/constraint.h"
#include "structs/vector2.h"
#include "structs/color.h"

struct RectWorld : public World
{
public:
	RectWorld(Color background, Vector2 center, Vector2 size, Color color);
	void Render() override;
	Vector2 Center() const override;
	bool Contains(Vector2 point) const override;
	void Contrain(Vector2& pos, Particle& p) const override;
	std::pair<Vector2, Vector2> Bounds() const override;

private:
	Vector2 center;
	Vector2 extends;
	Color color;
};
