#pragma once
#include "structs/vector2.h"

class SceneObject
{
public:
	SceneObject(Vector2 position) : position(position) { }
	virtual ~SceneObject() { }

private:
	Vector2 position;
};
