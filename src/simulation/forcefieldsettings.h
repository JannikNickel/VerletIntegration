#pragma once
#include "structs/vector2.h"
#include "structs/color.h"
#include "structs/gradient.h"

enum class ForceFieldShape
{
	Circle,
	Rect
};

enum class ForceFieldFalloff
{
	Linear,
	Quadratic,
	Cubic,
	Inverse,
	Smoothstep,
	None
};

enum class ForceFieldDirection
{
	FromCenter,
	Up,
	Left,
	Down,
	Right,
	Custom
};

struct ForceFieldSettings
{
	float force = 100.0f;
	bool massDependent = false;
	ForceFieldFalloff falloff = ForceFieldFalloff::Linear;
	ForceFieldShape shape = ForceFieldShape::Circle;
	float range = 100.0f;
	Vector2 rectSize = Vector2(200.0f, 200.0f);
	float rectRotation = 0.0f;
	ForceFieldDirection direction = ForceFieldDirection::FromCenter;
	float customDirection = 0.0;

	Vector2 DirVector() const
	{
		switch(direction)
		{
			case ForceFieldDirection::FromCenter:
				return Vector2(0.0f, 0.0f);
			case ForceFieldDirection::Up:		 
				return Vector2(0.0f, 1.0f);
			case ForceFieldDirection::Left: 
				return Vector2(-1.0f, 0.0f);
			case ForceFieldDirection::Down: 
				return Vector2(0.0f, -1.0f);
			case ForceFieldDirection::Right: 
				return Vector2(1.0f, 0.0f);
			case ForceFieldDirection::Custom: 
				return Vector2(cosf(customDirection * 0.0174533f), sinf(customDirection * 0.0174533f));
			default:
				return Vector2(0.0f, 0.0f);
		}
	};

	float ApplyFalloff(float x) const
	{
		switch(falloff)
		{
			case ForceFieldFalloff::Linear:
				return x;
			case ForceFieldFalloff::Quadratic:
				return x * x;
			case ForceFieldFalloff::Cubic:
				return x * x * x;
			case ForceFieldFalloff::Inverse:
				return 1.0f - (1.0f - x) * (1.0f - x);
			case ForceFieldFalloff::Smoothstep:
				return x * x * (3.0f - 2.0f * x);
			case ForceFieldFalloff::None:
				return 1.0f;
		}
		return x;
	}
};
