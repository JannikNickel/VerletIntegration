#include "circleworld.h"
#include "renderer/graphics.h"
#include "imgui.h"

CircleWorld::CircleWorld(Color background, Vector2 center, float radius, Color color) : World(background), center(center), radius(radius), color(color)
{

}

void CircleWorld::Render()
{
	World::Render();
	Graphics::Circle(center, radius, color);
}

Vector2 CircleWorld::Center() const
{
	return center;
}

void CircleWorld::Contrain(Vector2& pos, const Particle& p) const
{
	Vector2 dir = pos - center;
	float dst = dir.SqrLength();
	float rad = radius - p.radius;
	if(dst > rad * rad)
	{
		Vector2 normDir = dir / std::sqrtf(dst);
		pos = center + normDir * rad;
	}
}

std::pair<Vector2, Vector2> CircleWorld::Bounds() const
{
	Vector2 size = Vector2(radius, radius);
	return std::make_pair(center - size, center + size);
}
