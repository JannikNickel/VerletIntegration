#include "linkobject.h"
#include "engine/input.h"
#include "renderer/graphics.h"
#include "serialization/serializationhelper.h"
#include "utils/nameof.h"
#include "imgui.h"
#include "guihelper.h"
#include <algorithm>
#include <stdexcept>

void LinkObject::Render(float dt, const std::optional<Color>& color) const
{
    Vector2 mp = Input::MousePosition();
    Color c = (ignColTimer -= dt) > 0.0f ? this->color : color.value_or(this->color);
    if(p0.expired())
    {
        return;
    }
    Graphics::Line(p0.lock()->Position(), !p1.expired() ? p1.lock()->Position() : mp, c);
}

bool LinkObject::IsHovered(Vector2 mousePos) const
{
    if(p0.expired() || p1.expired())
    {
        return false;
    }

    Vector2 from = p0.lock()->Position();
    Vector2 to = p1.lock()->Position();
    Vector2 dir = to - from;
    float len = dir.Normalize();
    float dot = std::clamp(Vector2::Dot(mousePos - from, dir), 0.0f, len);
    return Vector2::Distance(from + dir * dot, mousePos) < 2.5f;
}

bool LinkObject::IsValid() const
{
    return !p0.expired() && !p1.expired();
}

EditResult LinkObject::Edit()
{
    ImGui::LabelText("", "Restrict Min");
    ImGui::Checkbox("##restrictMin", &restrictMin);

    ImGui::LabelText("", "Restrict Max");
    ImGui::Checkbox("##restrictMax", &restrictMax);

    ImGui::LabelText("", "Color");
    if(ImGui::ColorEdit4("##colorInput", &color.r, GuiHelper::defaultColorEditFlags))
    {
        ignColTimer = 2.0f;
    }

    ImGui::Spacing();
    int result = GuiHelper::HorizontalButtonSplit("Delete", "Duplicate");
    if(result > 0)
    {
        ignColTimer = 0.0;
    }
    return static_cast<EditResult>(result);
}

JsonObj LinkObject::Serialize() const
{
    JsonObj obj = SceneObject::Serialize();
    obj[NAMEOF(p0)] = p0.lock()->Id();
    obj[NAMEOF(p1)] = p1.lock()->Id();
    obj[NAMEOF(color)] = SerializationHelper::Serialize(color);
    obj[NAMEOF(restrictMin)] = restrictMin;
    obj[NAMEOF(restrictMax)] = restrictMax;
    return obj;
}

void LinkObject::Deserialize(const JsonObj& json)
{
    SceneObject::Deserialize(json);
    p0 = scene.FindObject(json[NAMEOF(p0)]);
    p1 = scene.FindObject(json[NAMEOF(p1)]);
    color = SerializationHelper::Deserialize<Color>(json[NAMEOF(color)]);
    restrictMin = json[NAMEOF(restrictMin)];
    restrictMax = json[NAMEOF(restrictMax)];
}

std::weak_ptr<SceneObject>& LinkObject::operator[](size_t index)
{
    switch(index)
    {
        case 0:
            return p0;
        case 1:
            return p1;
        default:
            throw std::out_of_range("");
    }
}
