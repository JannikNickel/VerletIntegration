#pragma once
#include <cstdint>
#include <type_traits>

using ComponentId = uint32_t;

struct ComponentBase
{
	virtual ComponentId CompId() const = 0;
	virtual size_t Size() const = 0;

	static ComponentId RegisterComponent();
};

template <class T>
struct Component : public ComponentBase
{
	static const ComponentId componentId;

	ComponentId CompId() const override
	{
		return componentId;
	}

	size_t Size() const override
	{
		return sizeof(T);
	}
};

template<typename T>
concept ComponentDerived = std::is_base_of<Component<T>, T>::value;

template<class T>
const uint32_t Component<T>::componentId = ComponentBase::RegisterComponent();
