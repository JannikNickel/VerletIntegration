#pragma once
#include "serialization/serializable.h"
#include "structs/vector2.h"
#include "structs/color.h"
#include "core/world.h"

enum class WorldShape
{
	Rect,
	Circle
};

union WorldDimensions
{
	Vector2 size;
	float radius;
};

struct WorldData : public ISerializable
{
	WorldShape shape;
	WorldDimensions bounds;
	Color background;
	Color color;

	WorldData(WorldShape shape, WorldDimensions bounds, const Color& background, const Color& color);
	WorldData();

	JsonObj Serialize() const override;
	void Deserialize(const JsonObj& json) override;

};
