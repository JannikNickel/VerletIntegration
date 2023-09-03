#include "sceneobject.h"
#include "serialization/serializationhelper.h"
#include "utils/nameof.h"
#include "magic_enum.hpp"

JsonObj SceneObject::Serialize() const
{
	JsonObj json = {};
	json[ISerializable::typeKey] = magic_enum::enum_name(ObjType());
	json[NAMEOF(id)] = id;
	json[NAMEOF(position)] = SerializationHelper::Serialize(position);
	return json;
}

void SceneObject::Deserialize(const JsonObj& json)
{
	id = json[NAMEOF(id)];
	position = SerializationHelper::Deserialize<Vector2>(json[NAMEOF(position)]);
}
