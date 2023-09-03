#pragma once
#include "structs/vector2.h"
#include "structs/color.h"
#include "serialization/serializable.h"
#include <optional>
#include <memory>
#include <cstdint>

enum class EditResult
{
	None,
	Delete,
	Duplicate
};

enum class SceneObjectType
{
	Particle,
	Spawner,
	Link
};

class SceneObject : public ISerializable
{
	friend class Editor;
	friend class Scene;

public:
	SceneObject(Vector2 position) : position(position) { }
	SceneObject() : position(Vector2::zero) { }
	virtual ~SceneObject() { }

	uint32_t Id() const { return id; };
	Vector2 Position() const { return position; }

	virtual SceneObjectType ObjType() const = 0;
	virtual void Render(float dt, const std::optional<Color>& color) const = 0;
	virtual bool IsHovered(Vector2 mousePos) const = 0;
	virtual bool IsValid() const { return true; }
	virtual EditResult Edit() = 0;

	virtual std::unique_ptr<SceneObject> Clone() const = 0;
	JsonObj Serialize() const override;
	void Deserialize(const JsonObj& json) override;

protected:
	uint32_t id = 0;
	Vector2 position;
};

template<typename T>
class CloneableSceneObject : public SceneObject
{
public:
	using SceneObject::SceneObject;

	std::unique_ptr<SceneObject> Clone() const override
	{
		std::unique_ptr<CloneableSceneObject> obj = std::make_unique<T>(static_cast<const T&>(*this));
		obj->id = 0;
		return obj;
	}
};
