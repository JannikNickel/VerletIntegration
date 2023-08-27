#pragma once
#include "structs/vector2.h"
#include "structs/color.h"

class SceneObject
{
	friend class Editor;

public:
	SceneObject(Vector2 position) : position(position) { }
	virtual ~SceneObject() { }

	virtual const char* ObjectName() const = 0;
	virtual void Render(const Color& color) const = 0;
	virtual bool IsHovered(Vector2 mousePos) const = 0;
	virtual void Edit() = 0;

protected:
	Vector2 position;
};
