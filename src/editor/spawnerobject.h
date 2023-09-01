#pragma once
#include "sceneobject.h"
#include "simulation/spawnersettings.h"
#include "serialization/serializationhelper.h"

class SpawnerObject : public CloneableSceneObject<SpawnerObject>
{
public:
	static inline const float size = 20.0f;

	SpawnerObject(Vector2 position) : CloneableSceneObject(position) { }
	SpawnerObject() { }

	const SpawnerSettings& Settings() const { return settings; }

	SceneObjectType ObjType() const override { return SceneObjectType::Spawner; }
	void Render(float dt, const std::optional<Color>& color) const override;
	bool IsHovered(Vector2 mousePos) const override;
	EditResult Edit() override;

	JsonObj Serialize() const override;
	void Deserialize(const JsonObj& json) override;

private:
	SpawnerSettings settings;
};
