#pragma once
#include "sceneobject.h"
#include "renderer/graphics.h"

class ParticleObject : public SceneObject
{
public:
	ParticleObject(Vector2 position) : SceneObject(position)
	{
		
	}

	void Render(const Color& color) const override
	{
		Graphics::Circle(position, 10.0f, color);
	}
};
