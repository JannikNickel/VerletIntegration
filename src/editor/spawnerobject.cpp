#include "spawnerobject.h"
#include "particleobject.h"
#include "renderer/graphics.h"
#include "utils/nameof.h"
#include "imgui.h"
#include "guihelper.h"
#include "magic_enum.hpp"
#include <algorithm>

void SpawnerObject::Render(float dt, const std::optional<Color>& color) const
{
	static const Texture tex = Texture("resources/spawner.png");
	static const float spawnDirLen = 2.0f;
	static const int arcSegments = 16;

	Color c = color.value_or(Color::white);
	if(settings.spawnDirectionVariation == 0.0f)
	{
		Graphics::Line(position, position + settings.SpawnDirVector() * size * spawnDirLen, c);
	}
	else
	{
		float step = settings.spawnDirectionVariation / arcSegments;
		float current = -settings.spawnDirectionVariation * 0.5f;
		Vector2 prev = position;
		for(int i = 0; i <= arcSegments; i++)
		{
			Vector2 p = position + settings.SpawnDirVector(current) * size * spawnDirLen;
			Graphics::Line(prev, p, c);
			current += step;
			prev = p;
		}
		Graphics::Line(prev, position, c);
	}

	Graphics::Quad(position, Vector2::one * size, c, tex);
}

bool SpawnerObject::IsHovered(Vector2 mousePos) const
{
	return Vector2::Distance(position, mousePos) <= size;
}

EditResult SpawnerObject::Edit()
{
	static int currentNode = 0;

	ImGui::SetNextItemOpen(currentNode == 0, ImGuiCond_Always);
	if(ImGui::CollapsingHeader("SPAWNING", ImGuiTreeNodeFlags_DefaultOpen))
	{
		currentNode = 0;

		ImGui::LabelText("", "Spawn rate");
		GuiHelper::ClampedFloatInput("##spawnRate", &settings.spawnRate, "%0.3f", 0.0f, 60.0f);

		ImGui::Checkbox("Scale spawn rate", &settings.scaleSpawnRate);

		ImGui::LabelText("", "Initial delay");
		GuiHelper::ClampedFloatInput("##initialDelay", &settings.initialDelay, "%0.2f", 0.0f, 300.0f);

		ImGui::LabelText("", "Spawn condition");
		GuiHelper::EnumDropdown("##spawnCondition", &settings.spawnCondition);

		if(settings.spawnCondition != SpawnCondition::Always)
		{
			ImGui::LabelText("", "Condition value");
			GuiHelper::ClampedFloatInput("##spawnConditionValue", &settings.spawnConditionValue, "%0.2f", 1.0f, 1000000.0f);
		}
	}

	ImGui::SetNextItemOpen(currentNode == 1, ImGuiCond_Always);
	if(ImGui::CollapsingHeader("PARTICLES"))
	{
		currentNode = 1;

		ImGui::LabelText("", "Particle size (min, max)");
		GuiHelper::ClampedFloat2Input("##pSize", &settings.pSize.x, "%0.2f", ParticleObject::minSize, ParticleObject::maxSize, true);

		ImGui::LabelText("", "Particle mass");
		GuiHelper::ClampedFloatInput("##pMass", &settings.pMass, "%0.2f", ParticleObject::minMass, ParticleObject::maxMass);

		ImGui::Checkbox("Scale particle mass", &settings.scalePMass);

		ImGui::LabelText("", "Particle bounciness");
		GuiHelper::ClampedFloatInput("##pBounciness", &settings.pBounciness, "%0.2f", 0.0f, 1.0f);

		ImGui::Spacing();
	}

	ImGui::SetNextItemOpen(currentNode == 2, ImGuiCond_Always);
	if(ImGui::CollapsingHeader("COLORS"))
	{
		currentNode = 2;

		ImGui::LabelText("", "Color mode");
		GuiHelper::EnumDropdown("##pColorMode", &settings.pColorMode);

		if(settings.pColorMode == SpawnColorMode::RandomGradient || settings.pColorMode == SpawnColorMode::ShiftGradient)
		{
			ImGui::LabelText("", "Gradient");
			GuiHelper::GradientEdit("##pColorGradient", &settings.pColorGradient);
		}
		else
		{
			ImGui::LabelText("", "Color");
			ImGui::ColorEdit4("##pColorSingle", &settings.pColorSingle.r, GuiHelper::defaultColorEditFlags);
		}

		if(settings.pColorMode != SpawnColorMode::Fixed && settings.pColorMode != SpawnColorMode::RandomHue && settings.pColorMode != SpawnColorMode::RandomGradient)
		{
			ImGui::LabelText("", "Repeat mode");
			GuiHelper::EnumDropdown("##colorRepeatMode", &settings.pColorShift);

			ImGui::LabelText("", "Animation duration");
			GuiHelper::ClampedFloatInput("##colorShiftDuration", &settings.pColorShiftDuration, "%0.2f", 0.1f, 300.0f);
		}

		ImGui::Spacing();
	}

	ImGui::SetNextItemOpen(currentNode == 3, ImGuiCond_Always);
	if(ImGui::CollapsingHeader("FORCE"))
	{
		currentNode = 3;

		ImGui::LabelText("", "Spawn direction");
		ImGui::SliderFloat("##spawnDirection", &settings.spawnDirection, -360.0f, 360.0f, "%0.2f");

		ImGui::LabelText("", "Spawn variation");
		ImGui::SliderFloat("##spawnVariation", &settings.spawnDirectionVariation, 0.0f, 360.0f, "%0.2f");

		ImGui::LabelText("", "Spawn force (min, max)");
		GuiHelper::ClampedFloat2Input("##spawnForce", &settings.spawnForce.x, "%0.2f", 0.0f, 10000.0f, true);

		ImGui::Checkbox("Scale spawn force", &settings.scaleSpawnForce);

		ImGui::LabelText("", "Direction mode");
		GuiHelper::EnumDropdown("##directionMode", &settings.spawnDirectionRotation);

		if(settings.spawnDirectionRotation != SpawnDirectionRotation::Fixed)
		{
			ImGui::LabelText("", "Repeat mode");
			GuiHelper::EnumDropdown("##spawnRepeatMode", &settings.spawnDirectionRotationRepeat);

			ImGui::LabelText("", "Anim start");
			GuiHelper::ClampedFloatInput("##spawnDirStart", &settings.spawnDirectionRotationStart, "%0.2f", 0.0f, 1.0f);

			ImGui::LabelText("", "Animation duration");
			GuiHelper::ClampedFloatInput("##spawnDirDuration", &settings.spawnDirectionRotationDuration, "%0.2f", 0.1f, 300.0f);

			ImGui::LabelText("", "Rotation limit");
			GuiHelper::ClampedFloatInput("##spawnDirRotationLimit", &settings.spawnDirectionRotationLimit, "%0.2f", -360.0f, 360.0f);
		}

		ImGui::Spacing();
	}

	ImGui::Spacing();
	int result = GuiHelper::HorizontalButtonSplit("Delete", "Duplicate");
	return static_cast<EditResult>(result);
}

JsonObj SpawnerObject::Serialize() const
{
	JsonObj json = SceneObject::Serialize();
	json[NAMEOF(settings.spawnRate)] = settings.spawnRate;
	json[NAMEOF(settings.scaleSpawnRate)] = settings.scaleSpawnRate;
	json[NAMEOF(settings.initialDelay)] = settings.initialDelay;
	json[NAMEOF(settings.spawnCondition)] = magic_enum::enum_name(settings.spawnCondition);
	json[NAMEOF(settings.spawnConditionValue)] = settings.spawnConditionValue;
	json[NAMEOF(settings.pSize)] = SerializationHelper::Serialize(settings.pSize);
	json[NAMEOF(settings.pMass)] = settings.pMass;
	json[NAMEOF(settings.scalePMass)] = settings.scalePMass;
	json[NAMEOF(settings.pBounciness)] = settings.pBounciness;
	json[NAMEOF(settings.pColorMode)] = magic_enum::enum_name(settings.pColorMode);
	json[NAMEOF(settings.pColorSingle)] = SerializationHelper::Serialize(settings.pColorSingle);
	json[NAMEOF(settings.pColorGradient)] = SerializationHelper::Serialize(settings.pColorGradient);
	json[NAMEOF(settings.pColorShift)] = magic_enum::enum_name(settings.pColorShift);
	json[NAMEOF(settings.pColorShiftDuration)] = settings.pColorShiftDuration;
	json[NAMEOF(settings.spawnDirection)] = settings.spawnDirection;
	json[NAMEOF(settings.spawnDirectionVariation)] = settings.spawnDirectionVariation;
	json[NAMEOF(settings.spawnForce)] = SerializationHelper::Serialize(settings.spawnForce);
	json[NAMEOF(settings.scaleSpawnForce)] = settings.scaleSpawnForce;
	json[NAMEOF(settings.spawnDirectionRotation)] = magic_enum::enum_name(settings.spawnDirectionRotation);
	json[NAMEOF(settings.spawnDirectionRotationRepeat)] = magic_enum::enum_name(settings.spawnDirectionRotationRepeat);
	json[NAMEOF(settings.spawnDirectionRotationStart)] = settings.spawnDirectionRotationStart;
	json[NAMEOF(settings.spawnDirectionRotationDuration)] = settings.spawnDirectionRotationDuration;
	json[NAMEOF(settings.spawnDirectionRotationLimit)] = settings.spawnDirectionRotationLimit;
	return json;
}

void SpawnerObject::Deserialize(const JsonObj& json)
{
	SceneObject::Deserialize(json);
	settings.spawnRate = json[NAMEOF(settings.spawnRate)];
	settings.scaleSpawnRate = json[NAMEOF(settings.scaleSpawnRate)];
	settings.initialDelay = json[NAMEOF(settings.initialDelay)];
	settings.spawnCondition = magic_enum::enum_cast<SpawnCondition>(static_cast<std::string>(json[NAMEOF(settings.spawnCondition)])).value();
	settings.spawnConditionValue = json[NAMEOF(settings.spawnConditionValue)];
	settings.pSize = SerializationHelper::Deserialize<Vector2>(json[NAMEOF(settings.pSize)]);
	settings.pMass = json[NAMEOF(settings.pMass)];
	settings.scalePMass = json[NAMEOF(settings.scalePMass)];
	settings.pBounciness = json[NAMEOF(settings.pBounciness)];
	settings.pColorMode = magic_enum::enum_cast<SpawnColorMode>(static_cast<std::string>(json[NAMEOF(settings.pColorMode)])).value();
	settings.pColorSingle = SerializationHelper::Deserialize<Color>(json[NAMEOF(settings.pColorSingle)]);
	settings.pColorGradient = SerializationHelper::Deserialize<Gradient>(json[NAMEOF(settings.pColorGradient)]);
	settings.pColorShift = magic_enum::enum_cast<SpawnRepeatMode>(static_cast<std::string>(json[NAMEOF(settings.pColorShift)])).value();
	settings.pColorShiftDuration = json[NAMEOF(settings.pColorShiftDuration)];
	settings.spawnDirection = json[NAMEOF(settings.spawnDirection)];
	settings.spawnDirectionVariation = json[NAMEOF(settings.spawnDirectionVariation)];
	settings.spawnForce = SerializationHelper::Deserialize<Vector2>(json[NAMEOF(settings.spawnForce)]);
	settings.scaleSpawnForce = json[NAMEOF(settings.scaleSpawnForce)];
	settings.spawnDirectionRotation = magic_enum::enum_cast<SpawnDirectionRotation>(static_cast<std::string>(json[NAMEOF(settings.spawnDirectionRotation)])).value();
	settings.spawnDirectionRotationRepeat = magic_enum::enum_cast<SpawnRepeatMode>(static_cast<std::string>(json[NAMEOF(settings.spawnDirectionRotationRepeat)])).value();
	settings.spawnDirectionRotationStart = json[NAMEOF(settings.spawnDirectionRotationStart)];
	settings.spawnDirectionRotationDuration = json[NAMEOF(settings.spawnDirectionRotationDuration)];
	settings.spawnDirectionRotationLimit = json[NAMEOF(settings.spawnDirectionRotationLimit)];
}
