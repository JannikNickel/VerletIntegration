#pragma once
#include "structs/vector2.h"
#include "structs/color.h"
#include <optional>
#include <memory>

enum class EditResult
{
	None,
	Delete,
	Duplicate
};

class SceneObject
{
	friend class Editor;

public:
	SceneObject(Vector2 position) : position(position) { }
	virtual ~SceneObject() { }

	virtual const char* ObjectName() const = 0;
	virtual void Render(float dt, const std::optional<Color>& color) const = 0;
	virtual bool IsHovered(Vector2 mousePos) const = 0;
	virtual EditResult Edit() = 0;

	virtual std::unique_ptr<SceneObject> Clone() const = 0;

protected:
	Vector2 position;
};

template<typename T>
class CloneableSceneObject : public SceneObject
{
public:
	using SceneObject::SceneObject;

	std::unique_ptr<SceneObject> Clone() const override
	{
		return std::make_unique<T>(static_cast<const T&>(*this));
	}
};
