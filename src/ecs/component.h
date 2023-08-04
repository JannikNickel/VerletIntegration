#pragma once
#include <cstdint>
#include <type_traits>

using ComponentId = uint32_t;

struct ComponentBase
{
	static ComponentId RegisterComponent();
};

template <class T>
struct Component : public ComponentBase
{
	static const ComponentId componentId;
};

template<typename T>
concept ComponentDerived = std::is_base_of<Component<T>, T>::value;

template<class T>
const uint32_t Component<T>::componentId = ComponentBase::RegisterComponent();
