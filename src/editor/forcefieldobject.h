#pragma once
#include "sceneobject.h"
#include "serialization/serializationhelper.h"
#include "simulation/forcefieldsettings.h"
#include "structs/color.h"

class ForceFieldObject : public CloneableSceneObject<ForceFieldObject>
{
	friend class Scene;

public:
	static inline const float size = 20.0f;
	static inline const float minSize = 50.0;
	static inline const float maxSize = 1000.0;

	ForceFieldObject(Vector2 position) : CloneableSceneObject(position) {}
	ForceFieldObject() {}

	const ForceFieldSettings& Settings() const { return settings; }

	SceneObjectType ObjType() const override { return SceneObjectType::ForceField; }
	void Render(float dt, const std::optional<Color>& color) const override;
	bool IsHovered(Vector2 mousePos) const override;
	EditResult Edit() override;

	JsonObj Serialize() const override;
	void Deserialize(const JsonObj& json) override;

private:
	ForceFieldSettings settings;
};
