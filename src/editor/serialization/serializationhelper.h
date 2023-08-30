#pragma once
#include "serializable.h"
#include "utils/nameof.h"
#include "structs/vector2.h"
#include "structs/color.h"

namespace SerializationHelper
{
	inline JsonObj Serialize(Vector2 value)
	{
		return JsonObj
		{
			{ NAMEOF(value.x), value.x },
			{ NAMEOF(value.y), value.y }
		};
	}

	inline JsonObj Serialize(const Color& value)
	{
		return JsonObj
		{
			{ NAMEOF(value.r), value.r },
			{ NAMEOF(value.g), value.g },
			{ NAMEOF(value.b), value.b },
			{ NAMEOF(value.a), value.a }
		};
	}

	template<typename T> requires std::same_as<T, Color> || std::same_as<T, Vector2>
	inline T Deserialize(const JsonObj& json)
	{
		return T();
	}

	template<>
	inline Color Deserialize<Color>(const JsonObj& json)
	{
		return Color(json[NAMEOF(Color::r)], json[NAMEOF(Color::g)], json[NAMEOF(Color::b)], json[NAMEOF(Color::a)]);
	}

	template<>
	inline Vector2 Deserialize<Vector2>(const JsonObj& json)
	{
		return Vector2(json[NAMEOF(Vector2::x)], json[NAMEOF(Vector2::y)]);
	}
}
