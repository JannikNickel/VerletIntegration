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
	ImGui::LabelText("", "Spawn rate");
	GuiHelper::ClampedFloatInput("##spawnRate", &settings.spawnRate, "%0.2f", 0.0f, 60.0f);

	ImGui::Checkbox("Scale spawn rate", &settings.scaleSpawnRate);

	if(ImGui::CollapsingHeader("PARTICLES"))
	{
		ImGui::LabelText("", "Particle size (min, max)");
		if(GuiHelper::ClampedFloat2Input("##pSize", &settings.pSize.x, "%0.2f", ParticleObject::minSize, ParticleObject::maxSize));

		ImGui::LabelText("", "Particle mass");
		GuiHelper::ClampedFloatInput("##pMass", &settings.pMass, "%0.2f", ParticleObject::minMass, ParticleObject::maxMass);

		ImGui::Checkbox("Scale particle mass", &settings.scalePMass);

		ImGui::LabelText("", "Particle bounciness");
		GuiHelper::ClampedFloatInput("##pBounciness", &settings.pBounciness, "%0.2f", 0.0f, 1.0f);

		ImGui::Spacing();
	}

	if(ImGui::CollapsingHeader("COLORS"))
	{
		ImGui::LabelText("", "Color mode");
		GuiHelper::EnumDropdown("##pColorMode", &settings.pColorMode);

		ImGui::LabelText("", "Color");
		ImGui::ColorEdit4("##pColorSingle", &settings.pColorSingle.r, ImGuiColorEditFlags_DisplayHex);

		if(settings.pColorMode != SpawnColorMode::Fixed && settings.pColorMode != SpawnColorMode::RandomHue)
		{
			ImGui::LabelText("", "Repeat mode");
			GuiHelper::EnumDropdown("##colorRepeatMode", &settings.pColorShift);

			ImGui::LabelText("", "Animation duration");
			GuiHelper::ClampedFloatInput("##colorShiftDuration", &settings.pColorShiftDuration, "%0.2f", 0.1f, 300.0f);
		}

		ImGui::Spacing();
	}

	if(ImGui::CollapsingHeader("FORCE"))
	{
		ImGui::LabelText("", "Spawn direction");
		ImGui::SliderFloat("##spawnDirection", &settings.spawnDirection, -360.0f, 360.0f, "%0.2f");

		ImGui::LabelText("", "Spawn variation");
		ImGui::SliderFloat("##spawnVariation", &settings.spawnDirectionVariation, 0.0f, 360.0f, "%0.2f");

		ImGui::LabelText("", "Spawn force (min, max)");
		GuiHelper::ClampedFloat2Input("##spawnForce", &settings.spawnForce.x, "%0.2f", 0.0f, 10000.0f);

		ImGui::Checkbox("Scale spawn force", &settings.scaleSpawnForce);

		ImGui::LabelText("", "Direction mode");
		GuiHelper::EnumDropdown("##directionMode", &settings.spawnDirectionMode);

		if(settings.spawnDirectionMode != SpawnDirectionMode::Fixed)
		{
			ImGui::LabelText("", "Repeat mode");
			GuiHelper::EnumDropdown("##spawnRepeatMode", &settings.spawnDirectionRepeatMode);

			ImGui::LabelText("", "Animation duration");
			GuiHelper::ClampedFloatInput("##spawnDirDuration", &settings.spawnDirectionDuration, "%0.2f", 0.1f, 300.0f);
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
	json[NAMEOF(settings.pSize)] = SerializationHelper::Serialize(settings.pSize);
	json[NAMEOF(settings.pMass)] = settings.pMass;
	json[NAMEOF(settings.scalePMass)] = settings.scalePMass;
	json[NAMEOF(settings.pBounciness)] = settings.pBounciness;
	json[NAMEOF(settings.pColorMode)] = magic_enum::enum_name(settings.pColorMode);
	json[NAMEOF(settings.pColorSingle)] = SerializationHelper::Serialize(settings.pColorSingle);
	json[NAMEOF(settings.pColorShift)] = magic_enum::enum_name(settings.pColorShift);
	json[NAMEOF(settings.pColorShiftDuration)] = settings.pColorShiftDuration;
	json[NAMEOF(settings.spawnDirection)] = settings.spawnDirection;
	json[NAMEOF(settings.spawnDirectionVariation)] = settings.spawnDirectionVariation;
	json[NAMEOF(settings.spawnForce)] = SerializationHelper::Serialize(settings.spawnForce);
	json[NAMEOF(settings.scaleSpawnForce)] = settings.scaleSpawnForce;
	json[NAMEOF(settings.spawnDirectionMode)] = magic_enum::enum_name(settings.spawnDirectionMode);
	json[NAMEOF(settings.spawnDirectionRepeatMode)] = magic_enum::enum_name(settings.spawnDirectionRepeatMode);
	json[NAMEOF(settings.spawnDirectionDuration)] = settings.spawnDirectionDuration;
	return json;
}

void SpawnerObject::Deserialize(const JsonObj& json)
{
	SceneObject::Deserialize(json);
	settings.spawnRate = json[NAMEOF(settings.spawnRate)];
	settings.scaleSpawnRate = json[NAMEOF(settings.scaleSpawnRate)];
	settings.pSize = SerializationHelper::Deserialize<Vector2>(json[NAMEOF(settings.pSize)]);
	settings.pMass = json[NAMEOF(settings.pMass)];
	settings.scalePMass = json[NAMEOF(settings.scalePMass)];
	settings.pBounciness = json[NAMEOF(settings.pBounciness)];
	settings.pColorMode = magic_enum::enum_cast<SpawnColorMode>(static_cast<std::string>(json[NAMEOF(settings.pColorMode)])).value();
	settings.pColorSingle = SerializationHelper::Deserialize<Color>(json[NAMEOF(settings.pColorSingle)]);
	settings.pColorShift = magic_enum::enum_cast<SpawnRepeatMode>(static_cast<std::string>(json[NAMEOF(settings.pColorShift)])).value();
	settings.pColorShiftDuration = json[NAMEOF(settings.pColorShiftDuration)];
	settings.spawnDirection = json[NAMEOF(settings.spawnDirection)];
	settings.spawnDirectionVariation = json[NAMEOF(settings.spawnDirectionVariation)];
	settings.spawnForce = SerializationHelper::Deserialize<Vector2>(json[NAMEOF(settings.spawnForce)]);
	settings.scaleSpawnForce = json[NAMEOF(settings.scaleSpawnForce)];
	settings.spawnDirectionMode = magic_enum::enum_cast<SpawnDirectionMode>(static_cast<std::string>(json[NAMEOF(settings.spawnDirectionMode)])).value();
	settings.spawnDirectionRepeatMode = magic_enum::enum_cast<SpawnRepeatMode>(static_cast<std::string>(json[NAMEOF(settings.spawnDirectionRepeatMode)])).value();
	settings.spawnDirectionDuration = json[NAMEOF(settings.spawnDirectionDuration)];
}
