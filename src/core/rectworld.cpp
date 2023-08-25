#include "rectworld.h"
#include "renderer/graphics.h"
#include "utils/math.h"
#include <cmath>

RectWorld::RectWorld(Color background, Vector2 center, Vector2 size, Color color) : World(background), center(center), extends(size * 0.5f), color(color)
{

}

void RectWorld::Render()
{
	World::Render();
	Graphics::Quad(center, extends * 2.0f, color);
}

Vector2 RectWorld::Center() const
{
	return center;
}

bool RectWorld::Contains(Vector2 point) const
{
	Vector2 min = center - extends;
	Vector2 max = center + extends;
	return min.x < point.x && max.x > point.x && min.y < point.y && max.y > point.y;
}

void RectWorld::Contrain(Vector2& pos, Particle& p) const
{
	float xDiff = pos.x - center.x;
	float extX = extends.x - p.radius;
	if(std::fabsf(xDiff) > extX)
	{
		Vector2 vel = pos - p.prevPos;
		float sgn = Math::Sgn(xDiff);
		pos.x = center.x + sgn * extX;
		p.prevPos = pos + Vector2(vel.x * p.bounciness, -vel.y);
	}

	float yDiff = pos.y - center.y;
	float extY = extends.y - p.radius;
	if(std::fabsf(yDiff) > extY)
	{
		Vector2 vel = pos - p.prevPos;
		float sgn = Math::Sgn(yDiff);
		pos.y = center.y + sgn * extY;
		p.prevPos = pos + Vector2(-vel.x, vel.y * p.bounciness);
	}
}

std::pair<Vector2, Vector2> RectWorld::Bounds() const
{
	return std::make_pair(center - extends, center + extends);
}
