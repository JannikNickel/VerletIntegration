#include "worlddata.h"
#include "serialization/serializationhelper.h"
#include "utils/nameof.h"
#include "magic_enum.hpp"
#include <exception>

WorldData::WorldData(WorldShape shape, WorldDimensions bounds, const Color& background, const Color& color) : shape(shape), bounds(bounds), background(background), color(color)
{
	
}

JsonObj WorldData::Serialize() const
{
	JsonObj json = {};
	json[NAMEOF(shape)] = magic_enum::enum_name(shape);
	switch(shape)
	{
		case WorldShape::Rect:
			json[NAMEOF(bounds)] = SerializationHelper::Serialize(bounds.size);
			break;
		case WorldShape::Circle:
			json[NAMEOF(bounds)] = bounds.radius;
			break;
		default:
			throw std::exception("Unknown world shape!");
	}
	json[NAMEOF(background)] = SerializationHelper::Serialize(background);
	json[NAMEOF(color)] = SerializationHelper::Serialize(color);
	return json;
}

void WorldData::Deserialize()
{

}
