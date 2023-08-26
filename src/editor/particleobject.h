#pragma once
#include "sceneobject.h"
#include "renderer/graphics.h"

class ParticleObject : public SceneObject
{
public:
	float radius = 10.0f;

	ParticleObject(Vector2 position) : SceneObject(position)
	{
		
	}

	void Render(const Color& color) const override
	{
		Graphics::Circle(position, radius, color);
	}

	bool IsHovered(Vector2 mousePos) const override
	{
		return Vector2::Distance(position, mousePos) <= radius;
	}
};
