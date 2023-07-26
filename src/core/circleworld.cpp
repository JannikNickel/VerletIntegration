#include "circleworld.h"
#include "renderer/graphics.h"

CircleWorld::CircleWorld(Color background, Vector2 center, float radius, Color color) : World(background), center(center), radius(radius), color(color)
{

}

void CircleWorld::Render()
{
	World::Render();
	Graphics::Circle(center, radius, color);
}

Vector2 CircleWorld::Center()
{
	return center;
}

void CircleWorld::Contrain(VerletObj& obj)
{
	Vector2 dir = obj.pos - center;
	float dst = dir.Length();
	if(dst + obj.radius > radius)
	{
		Vector2 normDir = dir / dst;
		obj.pos = center + normDir * (radius - obj.radius);
	}
}
