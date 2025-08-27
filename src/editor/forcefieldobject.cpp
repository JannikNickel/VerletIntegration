#include "forcefieldobject.h"
#include "renderer/graphics.h"
#include "utils/nameof.h"
#include "imgui.h"
#include "guihelper.h"
#include <algorithm>

void ForceFieldObject::Render(float dt, const std::optional<Color>& color) const
{
	static const Texture tex = Texture("resources/forcefield.png");
	static const int arcSegments = 32;
	static const float arrowAngle = 25.0f;
	static const float arrowLength = 10.0f;

	Color c = color.value_or(Color::white);

	// Outline
	if(settings.shape == ForceFieldShape::Circle)
	{
		float step = 360.0f / arcSegments;
		float current = 0.0f;
		Vector2 dir = Vector2(1.0, 0.0);
		Vector2 prev = position + dir * settings.range;
		for(int i = 0; i <= arcSegments; i++)
		{
			Vector2 p = position + Vector2::Rotate(dir, current) * settings.range;
			Graphics::Line(prev, p, c);
			current += step;
			prev = p;
		}
	}
	else if(settings.shape == ForceFieldShape::Rect)
	{
		Vector2 rectExtends = settings.rectSize * 0.5f;
		float r = settings.rectRotation;
		Graphics::Line(position + Vector2::Rotate(Vector2(-rectExtends.x, -rectExtends.y), r), position + Vector2::Rotate(Vector2(rectExtends.x, -rectExtends.y), r), c);
		Graphics::Line(position + Vector2::Rotate(Vector2(rectExtends.x, -rectExtends.y), r), position + Vector2::Rotate(Vector2(rectExtends.x, rectExtends.y), r), c);
		Graphics::Line(position + Vector2::Rotate(Vector2(rectExtends.x, rectExtends.y), r), position + Vector2::Rotate(Vector2(-rectExtends.x, rectExtends.y), r), c);
		Graphics::Line(position + Vector2::Rotate(Vector2(-rectExtends.x, rectExtends.y), r), position + Vector2::Rotate(Vector2(-rectExtends.x, -rectExtends.y), r), c);
	}

	// Icon
	Graphics::Quad(position, Vector2::one * size, c, tex);

	// Force indicators
	Color cTransparent = c.WithAlpha(0.25f);

	Vector2 up = Vector2::up;
	Vector2 right = Vector2::right;
	if(settings.shape == ForceFieldShape::Rect)
	{
		up = Vector2::Rotate(up, settings.rectRotation);
		right = Vector2::Rotate(right, settings.rectRotation);
	}

	double dirIndicatorRange = settings.shape == ForceFieldShape::Circle ? settings.range : settings.rectSize.Length() * 0.5f;
	float margin = arrowLength * 1.5f;
	float shaftLen = std::fmin(dirIndicatorRange * 0.2f, std::fmax(15.0, arrowLength * 2.0f));
	float spacingLat = shaftLen * 2.5f;
	float spacingFlow = spacingLat;
	Vector2 halfRect = settings.shape == ForceFieldShape::Circle ? Vector2(dirIndicatorRange - margin, dirIndicatorRange - margin) : settings.rectSize * 0.5f - Vector2(margin, margin);

	auto isInside = [&](Vector2 p)
	{
		Vector2 local = p - position;
		if(settings.shape == ForceFieldShape::Circle)
		{
			return local.Length() <= dirIndicatorRange - margin;
		}
		Vector2 rectDir = Vector2::Rotate(local, -settings.rectRotation);
		return std::fabs(rectDir.x) <= halfRect.x && std::fabs(rectDir.y) <= halfRect.y;
	};

	int iSteps = static_cast<int>(std::floorf((dirIndicatorRange - margin) / spacingLat));
	int kSteps = static_cast<int>(std::floorf((dirIndicatorRange - margin) / spacingFlow));
	for(int i = -iSteps; i <= iSteps; i++)
	{
		for(int k = -kSteps; k <= kSteps; k++)
		{
			const Vector2 p = position + right * (i * spacingLat) + up * (k * spacingFlow);

			Vector2 localDir = settings.DirVector();
			if(settings.direction == ForceFieldDirection::FromCenter)
			{
				Vector2 d = p - position;
				float len = d.Length();
				if(len < 1e-5f)
				{
					continue;
				}
				localDir = d / len;
			}
			else if(settings.shape == ForceFieldShape::Rect)
			{
				localDir = Vector2::Rotate(localDir, settings.rectRotation);
			}

			if(settings.force < 0.0f)
			{
				localDir *= -1.0f;
			}
			Vector2 start = p - localDir * (shaftLen * 0.5f);
			Vector2 end = p + localDir * (shaftLen * 0.5f);
			if((i == 0 && k == 0) || (isInside(start) && isInside(end)))
			{
				Graphics::Arrow(start, end, arrowAngle, arrowLength, cTransparent);
			}
		}
	}
}

bool ForceFieldObject::IsHovered(Vector2 mousePos) const
{
	return Vector2::Distance(position, mousePos) <= size;
}

EditResult ForceFieldObject::Edit()
{
	ImGui::LabelText("", "Force");
	ImGui::SliderFloat("##force", &settings.force, -100000.0f, 100000.0f, "%0.3f");

	ImGui::LabelText("", "Mass Dependent");
	ImGui::Checkbox("##massDependent", &settings.massDependent);

	ImGui::LabelText("", "Falloff");
	GuiHelper::EnumDropdown("##falloff", &settings.falloff);

	ImGui::LabelText("", "Shape");
	GuiHelper::EnumDropdown("##shape", &settings.shape);

	if(settings.shape == ForceFieldShape::Circle)
	{
		ImGui::LabelText("", "Range");
		GuiHelper::ClampedFloatInput("##range", &settings.range, "%0.2f", minSize, maxSize);
	}
	else if(settings.shape == ForceFieldShape::Rect)
	{
		ImGui::LabelText("", "Size");
		GuiHelper::ClampedFloat2Input("##rectSize", &settings.rectSize.x, "%0.2f", minSize * 2.0f, maxSize * 2.0f);

		ImGui::LabelText("", "Rotation");
		ImGui::SliderFloat("##rectRotation", &settings.rectRotation, -360.0f, 360.0f, "%0.2f");
	}

	ImGui::LabelText("", "Direction");
	GuiHelper::EnumDropdown("##direction", &settings.direction);

	if(settings.direction == ForceFieldDirection::Custom)
	{
		ImGui::LabelText("", "Direction");
		ImGui::SliderFloat("##customDirection", &settings.customDirection, -360.0f, 360.0f, "%0.2f");
	}

	ImGui::Spacing();
	int result = GuiHelper::HorizontalButtonSplit("Delete", "Duplicate");
	return static_cast<EditResult>(result);
}

JsonObj ForceFieldObject::Serialize() const
{
	JsonObj json = SceneObject::Serialize();
	json[NAMEOF(settings.force)] = settings.force;
	json[NAMEOF(settings.massDependent)] = settings.massDependent;
	json[NAMEOF(settings.falloff)] = magic_enum::enum_name(settings.falloff);
	json[NAMEOF(settings.shape)] = magic_enum::enum_name(settings.shape);
	json[NAMEOF(settings.range)] = settings.range;
	json[NAMEOF(settings.rectSize)] = SerializationHelper::Serialize(settings.rectSize);
	json[NAMEOF(settings.rectRotation)] = settings.rectRotation;
	json[NAMEOF(settings.direction)] = magic_enum::enum_name(settings.direction);
	json[NAMEOF(settings.customDirection)] = settings.customDirection;
	return json;
}

void ForceFieldObject::Deserialize(const JsonObj& json)
{
	SceneObject::Deserialize(json);
	settings.force = json[NAMEOF(settings.force)];
	settings.massDependent = json[NAMEOF(settings.massDependent)];
	settings.falloff = magic_enum::enum_cast<ForceFieldFalloff>(static_cast<std::string>(json[NAMEOF(settings.falloff)])).value();
	settings.shape = magic_enum::enum_cast<ForceFieldShape>(static_cast<std::string>(json[NAMEOF(settings.shape)])).value();
	settings.range = json[NAMEOF(settings.range)];
	settings.rectSize = SerializationHelper::Deserialize<Vector2>(json[NAMEOF(settings.rectSize)]);
	settings.rectRotation = json[NAMEOF(settings.rectRotation)];
	settings.direction = magic_enum::enum_cast<ForceFieldDirection>(static_cast<std::string>(json[NAMEOF(settings.direction)])).value();
	settings.customDirection = json[NAMEOF(settings.customDirection)];
}
