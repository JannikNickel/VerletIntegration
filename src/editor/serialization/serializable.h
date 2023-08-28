#pragma once
#include "nlohmann/json.hpp"

using JsonObj = nlohmann::ordered_json;

class ISerializable
{
public:
	virtual ~ISerializable() { };

	virtual JsonObj Serialize() const = 0;
	virtual void Deserialize() = 0;
};
