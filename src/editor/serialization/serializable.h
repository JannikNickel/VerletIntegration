#pragma once
#include "nlohmann/json.hpp"
#include <string>

using JsonObj = nlohmann::ordered_json;

class ISerializable
{
public:
	static inline const std::string typeKey = "type";

	virtual ~ISerializable() { };

	virtual JsonObj Serialize() const = 0;
	virtual void Deserialize(const JsonObj& json) = 0;
};
