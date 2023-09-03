#include "physicsdata.h"
#include "utils/nameof.h"
#include "serialization/serializationhelper.h"
#include "guihelper.h"
#include "magic_enum.hpp"
#include "imgui.h"
#include <cmath>
#include <algorithm>

PhysicsData::PhysicsData(const SolverSettings& settings) : settings(settings)
{

}

PhysicsData::PhysicsData() : settings(SolverSettings())
{

}

const SolverSettings& PhysicsData::Settings() const
{
	return settings;
}

JsonObj PhysicsData::Serialize() const
{
	JsonObj json = {};
	json[NAMEOF(settings.updateMode)] = magic_enum::enum_name(settings.updateMode);
	json[NAMEOF(settings.timestep)] = settings.timestep;
	json[NAMEOF(settings.substeps)] = settings.substeps;
	json[NAMEOF(settings.gravity)] = SerializationHelper::Serialize(settings.gravity);
	json[NAMEOF(settings.collision)] = settings.collision;
	return json;
}

void PhysicsData::Deserialize(const JsonObj& json)
{
	settings.updateMode = magic_enum::enum_cast<SolverUpdateMode>(static_cast<std::string>(json[NAMEOF(settings.updateMode)])).value();
	settings.timestep = json[NAMEOF(settings.timestep)];
	settings.substeps = json[NAMEOF(settings.substeps)];
	settings.gravity = SerializationHelper::Deserialize<Vector2>(json[NAMEOF(settings.gravity)]);
	settings.collision = json[NAMEOF(settings.collision)];
}

void PhysicsData::Edit()
{
	ImGui::LabelText("", "Update mode");
	GuiHelper::EnumDropdown("##updateModeCombo", &settings.updateMode);

	ImGui::BeginDisabled(settings.updateMode == SolverUpdateMode::FrameDeltaTime);
	ImGui::LabelText("", "Simulation steps per second");
	int sps = static_cast<int>(std::roundf(1.0f / settings.timestep));
	if(ImGui::InputInt("##spsInput", &sps, 0, 0))
	{
		settings.timestep = 1.0f / static_cast<float>(std::clamp(sps, 10, 300));
	}
	ImGui::EndDisabled();

	ImGui::LabelText("", "Substeps");
	int substeps = settings.substeps;
	if(ImGui::InputInt("##substepsInput", &substeps, 0, 0))
	{
		settings.substeps = static_cast<uint32_t>(std::clamp(substeps, 1, 16));
	}

	ImGui::Spacing();
	ImGui::LabelText("", "Gravity");
	ImGui::InputFloat2("##gravityInput", &settings.gravity[0], "%.0f");

	ImGui::LabelText("", "Collisions");
	ImGui::Checkbox("##collisionsToggle", &settings.collision);
}
