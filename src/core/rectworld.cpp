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

void RectWorld::Contrain(Vector2& pos, const PhysicsCircle& p) const
{
	float xDiff = pos.x - center.x;
	float extX = extends.x - p.radius;
	if(std::fabsf(xDiff) > extX)
	{
		float sgn = Math::Sgn(xDiff);
		pos.x = center.x + sgn * extX;
	}

	float yDiff = pos.y - center.y;
	float extY = extends.y - p.radius;
	if(std::fabsf(yDiff) > extY)
	{
		float sgn = Math::Sgn(yDiff);
		pos.y = center.y + sgn * extY;
	}
}

std::pair<Vector2, Vector2> RectWorld::Bounds() const
{
	return std::make_pair(center - extends, center + extends);
}
