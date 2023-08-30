#pragma once
#include "sceneobject.h"
#include "renderer/graphics.h"
#include "utils/nameof.h"
#include "serialization/serializationhelper.h"
#include "imgui.h"
#include "guihelper.h"
#include <algorithm>

class ParticleObject : public CloneableSceneObject<ParticleObject>
{
public:
	float radius = 10.0f;
	float mass = 1.0f;
	float bounciness = 0.1f;
	Color color = Color::white;

	ParticleObject(Vector2 position) : CloneableSceneObject(position)
	{
		
	}

	ParticleObject()
	{
		
	}

	SceneObjectType ObjType() const override
	{
		return SceneObjectType::Particle;
	}

	void Render(float dt, const std::optional<Color>& color) const override
	{
		Graphics::Circle(position, radius, (ignColTimer -= dt) > 0.0f ? this->color : color.value_or(this->color));
	}

	bool IsHovered(Vector2 mousePos) const override
	{
		return Vector2::Distance(position, mousePos) <= radius;
	}

	EditResult Edit() override
	{
		ImGui::LabelText("", "Radius");
		GuiHelper::ClampedFloatInput("##radius", &radius, "%0.2f", 1.0f, 10.0f);

		ImGui::LabelText("", "Mass");
		GuiHelper::ClampedFloatInput("##massInput", &mass, "%0.2f", 0.01f, 10000.0f);

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

	JsonObj Serialize() const override
	{
		JsonObj json = SceneObject::Serialize();
		json[NAMEOF(radius)] = radius;
		json[NAMEOF(mass)] = mass;
		json[NAMEOF(bounciness)] = bounciness;
		json[NAMEOF(color)] = SerializationHelper::Serialize(color);
		return json;
	}

	void Deserialize(const JsonObj& json) override
	{
		SceneObject::Deserialize(json);
		radius = json[NAMEOF(radius)];
		mass = json[NAMEOF(mass)];
		bounciness = json[NAMEOF(bounciness)];
		color = SerializationHelper::Deserialize<Color>(json[NAMEOF(color)]);
	}

private:
	mutable float ignColTimer = 0.0;
};
