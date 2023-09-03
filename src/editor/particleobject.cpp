#include "particleobject.h"
#include "renderer/graphics.h"
#include "utils/nameof.h"
#include "serialization/serializationhelper.h"
#include "imgui.h"
#include "guihelper.h"
#include <algorithm>

void ParticleObject::Render(float dt, const std::optional<Color>& color) const
{
	Graphics::Circle(position, radius, (ignColTimer -= dt) > 0.0f ? this->color : color.value_or(this->color));
}

bool ParticleObject::IsHovered(Vector2 mousePos) const
{
	return Vector2::Distance(position, mousePos) <= radius;
}

EditResult ParticleObject::Edit()
{
	ImGui::LabelText("", "Radius");
	GuiHelper::ClampedFloatInput("##radius", &radius, "%0.2f", minSize, maxSize);

	ImGui::LabelText("", "Mass");
	GuiHelper::ClampedFloatInput("##massInput", &mass, "%0.2f", minMass, maxMass);

	ImGui::LabelText("", "Bounciness");
	GuiHelper::ClampedFloatInput("##bouncinessInput", &bounciness, "%0.2f", 0.0f, 1.0f);

	ImGui::LabelText("", "Color");
	if(ImGui::ColorEdit4("##colorInput", &color.r, ImGuiColorEditFlags_DisplayHex))
	{
		ignColTimer = 2.0f;
	}

	ImGui::Spacing();
	int result = GuiHelper::HorizontalButtonSplit("Delete", "Duplicate");
	if(result > 0.0f)
	{
		ignColTimer = 0.0;
	}
	return static_cast<EditResult>(result);
}

JsonObj ParticleObject::Serialize() const
{
	JsonObj json = SceneObject::Serialize();
	json[NAMEOF(radius)] = radius;
	json[NAMEOF(mass)] = mass;
	json[NAMEOF(bounciness)] = bounciness;
	json[NAMEOF(color)] = SerializationHelper::Serialize(color);
	return json;
}

void ParticleObject::Deserialize(const JsonObj& json)
{
	SceneObject::Deserialize(json);
	radius = json[NAMEOF(radius)];
	mass = json[NAMEOF(mass)];
	bounciness = json[NAMEOF(bounciness)];
	color = SerializationHelper::Deserialize<Color>(json[NAMEOF(color)]);
}
