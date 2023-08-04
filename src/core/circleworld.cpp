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

Vector2 CircleWorld::Center()
{
	return center;
}

void CircleWorld::Contrain(Vector2& pos, const PhysicsCircle& p)
{
	Vector2 dir = pos - center;
	float dst = dir.Length();
	if(dst + p.radius > radius)
	{
		Vector2 normDir = dir / dst;
		pos = center + normDir * (radius - radius);
	}
}
