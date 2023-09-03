#pragma once
#include "sceneobject.h"
#include "connectionplacement.h"
#include "serialization/serializable.h"
#include "structs/color.h"
#include "scene.h"
#include <memory>
#include <exception>

class LinkObject : public CloneableSceneObject<LinkObject>, public IConnectionPlacement
{
public:
	LinkObject(const Scene& scene, Vector2 position) : CloneableSceneObject(position), scene(scene) { }
	LinkObject(const Scene& scene) : scene(scene) { }

	const Color& Col() const { return color; }
	bool RestrictMin() const { return restrictMin; }
	bool RestrictMax() const { return restrictMax; }

	SceneObjectType ObjType() const override { return SceneObjectType::Link; }
	void Render(float dt, const std::optional<Color>& color) const override;
	bool IsHovered(Vector2 mousePos) const override;
	bool IsValid() const override;
	EditResult Edit() override;

	JsonObj Serialize() const override;
	void Deserialize(const JsonObj& json) override;

	std::weak_ptr<SceneObject>& operator[](size_t index) override;

private:
	const Scene& scene;
	std::weak_ptr<SceneObject> p0 = std::weak_ptr<SceneObject>();
	std::weak_ptr<SceneObject> p1 = std::weak_ptr<SceneObject>();

	Color color = Color::white;
	bool restrictMin = true;
	bool restrictMax = true;

	mutable float ignColTimer = 0.0;
};
