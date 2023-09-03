#pragma once
#include "sceneobject.h"

class IConnectionPlacement
{
public:
	virtual ~IConnectionPlacement() { };

	virtual std::weak_ptr<SceneObject>& operator[](size_t index) = 0;
};
