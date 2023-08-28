#include "sceneobject.h"
#include "serialization/serializationhelper.h"
#include "utils/nameof.h"

JsonObj SceneObject::Serialize() const
{
	JsonObj json = {};
	json["type"] = TypeIdentifier();
	json[NAMEOF(position)] = SerializationHelper::Serialize(position);
	return json;
}

void SceneObject::Deserialize()
{

}
