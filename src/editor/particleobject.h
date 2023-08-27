#pragma once
#include "sceneobject.h"
#include "renderer/graphics.h"
#include "imgui.h"
#include <algorithm>

class ParticleObject : public SceneObject
{
public:
	float radius = 10.0f;
	float mass = 1.0f;
	float bounciness = 0.1f;
	Color color = Color::white;

	ParticleObject(Vector2 position) : SceneObject(position)
	{
		
	}

	const char* ObjectName() const override
	{
		return "Particle";
	}

	void Render(const Color& color) const override
	{
		Graphics::Circle(position, radius, color);
	}

	bool IsHovered(Vector2 mousePos) const override
	{
		return Vector2::Distance(position, mousePos) <= radius;
	}

	void Edit() override
	{
		ImGui::LabelText("", "Radius");
		if(ImGui::InputFloat("##radiusInput", &radius, 0, 0, "%0.2f"))
		{
			radius = std::clamp(radius, 1.0f, 10.0f);
		}

		ImGui::LabelText("", "Mass");
		if(ImGui::InputFloat("##massInput", &mass, 0, 0, "%0.2f"))
		{
			mass = std::clamp(mass, 0.01f, 10000.0f);
		}

		ImGui::LabelText("", "Bounciness");
		if(ImGui::InputFloat("##bouncinessInput", &bounciness, 0, 0, "%0.2f"))
		{
			bounciness = std::clamp(bounciness, 0.0f, 1.0f);
		}

		ImGui::LabelText("", "Color");
		ImGui::ColorEdit4("##colorInput", &color.r, ImGuiColorEditFlags_InputHSV);
	}
};
