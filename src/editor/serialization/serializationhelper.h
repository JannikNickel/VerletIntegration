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
}
