#pragma once
#include "archetype.h"
#include <cstdint>

struct ComponentBase
{
	virtual ComponentId CompId() const = 0;
};

template <class T>
struct Component : public ComponentBase
{
	static const ComponentId componentId;

	ComponentId CompId() const override
	{
		return componentId;
	}
};

template<class T>
const uint32_t Component<T>::componentId = Archetype::RegisterComponent<T>();
