#pragma once
#include "sceneobject.h"
#include "serialization/serializable.h"
#include "structs/color.h"

class ParticleObject : public CloneableSceneObject<ParticleObject>
{
	friend class Scene;

public:
	static inline const float minSize = 1.0f;
	static inline const float maxSize = 10.0f;
	static inline const float minMass = 0.01f;
	static inline const float maxMass = 10000.0f;

	ParticleObject(Vector2 position, float radius, float mass, float bounciness, const Color& color, bool pinned)
		: CloneableSceneObject(position), radius(radius), mass(mass), bounciness(bounciness), color(color), pinned(pinned) { }
	ParticleObject(Vector2 position) : CloneableSceneObject(position) { }
	ParticleObject() { }

	SceneObjectType ObjType() const override { return SceneObjectType::Particle; }
	void Render(float dt, const std::optional<Color>& color) const override;
	bool IsHovered(Vector2 mousePos) const override;
	EditResult Edit() override;

	JsonObj Serialize() const override;
	void Deserialize(const JsonObj& json) override;

private:
	float radius = 10.0f;
	float mass = 1.0f;
	float bounciness = 0.1f;
	Color color = Color::white;
	bool pinned = false;

	mutable float ignColTimer = 0.0;
};
