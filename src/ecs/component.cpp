#include "component.h"

ComponentId ComponentBase::RegisterComponent()
{
	static ComponentId nextId = 0;
	return ++nextId;
}
