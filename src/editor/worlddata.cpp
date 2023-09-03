#include "worlddata.h"
#include "serialization/serializationhelper.h"
#include "utils/nameof.h"
#include "magic_enum.hpp"
#include <exception>

WorldData::WorldData(WorldShape shape, WorldDimensions bounds, const Color& background, const Color& color) : shape(shape), bounds(bounds), background(background), color(color)
{
	
}

WorldData::WorldData() : WorldData(WorldShape::Rect, WorldDimensions { Vector2::zero }, Color::black, Color::black)
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
			throw std::exception("Unknown WorldShape!");
	}
	json[NAMEOF(background)] = SerializationHelper::Serialize(background);
	json[NAMEOF(color)] = SerializationHelper::Serialize(color);
	return json;
}

void WorldData::Deserialize(const JsonObj& json)
{
	shape = magic_enum::enum_cast<WorldShape>(static_cast<std::string>(json[NAMEOF(shape)])).value();
	switch(shape)
	{
		case WorldShape::Rect:
			bounds = WorldDimensions { .size = SerializationHelper::Deserialize<Vector2>(json[NAMEOF(bounds)]) };
			break;
		case WorldShape::Circle:
			bounds = WorldDimensions { .radius = json[NAMEOF(bounds)] };
			break;
		default:
			throw std::exception("Unknown WorldShape!");
	}
	background = SerializationHelper::Deserialize<Color>(json[NAMEOF(background)]);
	color = SerializationHelper::Deserialize<Color>(json[NAMEOF(color)]);
}
