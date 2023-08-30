#pragma once
#include "physics/solversettings.h"
#include "editor/serialization/serializable.h"

class PhysicsData : public ISerializable
{
public:
	PhysicsData(const SolverSettings& settings);
	PhysicsData();

	const SolverSettings& Settings() const;

	JsonObj Serialize() const override;
	void Deserialize(const JsonObj& json) override;

	void Edit();

private:
	SolverSettings settings;
};
