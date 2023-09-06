#include "guihelper.h"
#include "utils/math.h"
#include "editorsettings.h"
#include <algorithm>

static void PushStyleColor(ImGuiCol col, const std::optional<Color>& color, bool copyAlpha)
{
	const ImVec4& defCol = ImGui::GetStyle().Colors[col];
	ImGui::PushStyleColor(col, color.has_value() ? ImVec4 { color.value().r, color.value().g, color.value().b, color.value().a * (copyAlpha ? defCol.w : 1.0f) } : defCol);
}

float GuiHelper::TitleBarHeight()
{
	const ImGuiStyle& style = ImGui::GetStyle();
	return ImGui::GetFontSize() + style.FramePadding.y * 2.0f;
}

void GuiHelper::CenterNextWindow()
{
	ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f }, ImGuiCond_Always, { 0.5f, 0.5f });
}

int GuiHelper::HorizontalButton(const char* button, float fullWidth, const std::optional<Color>& color, bool disabled)
{
	PushStyleColor(ImGuiCol_Button, color, true);
	PushStyleColor(ImGuiCol_ButtonActive, color, true);
	PushStyleColor(ImGuiCol_ButtonHovered, color, true);
	ImGui::BeginDisabled(disabled);
	bool result = ImGui::Button(button, { fullWidth, 0 });
	ImGui::EndDisabled();
	ImGui::PopStyleColor(3);
	return result;
}

int GuiHelper::HorizontalButtonSplit(const char* button0, const char* button1, const std::optional<Color>& b0Color, const std::optional<Color>& b1Color, bool b0Disabled, bool b1Disabled)
{
	float width = ImGui::GetContentRegionAvail().x;
	if(HorizontalButton(button0, width * 0.5f - ImGui::GetStyle().ItemInnerSpacing.x, b0Color, b0Disabled))
	{
		return 1;
	}
	ImGui::SameLine();
	if(HorizontalButton(button1, width * 0.5f - ImGui::GetStyle().ItemInnerSpacing.x, b1Color, b1Disabled))
	{
		return 2;
	}
	return 0;
}

bool GuiHelper::ClampedFloatInput(const char* label, float* value, const char* format, float min, float max)
{
	if(ImGui::InputFloat(label, value, 0.0f, 0.0f, format))
	{
		*value = std::clamp(*value, min, max);
		return true;
	}
	return false;
}

bool GuiHelper::ClampedFloat2Input(const char* label, float value[2], const char* format, float min, float max)
{
	if(ImGui::InputFloat2("##pSize", value, "%0.2f"))
	{
		value[0] = std::clamp(value[0], min, max);
		value[1] = std::clamp(value[1], min, max);
		if(value[0] > value[1])
		{
			std::swap(value[0], value[1]);
		}
		return true;
	}
	return false;
}

static void DrawGradient(ImVec2 min, ImVec2 max, Gradient* gradient)
{
	ImVec2 size = { max.x - min.x, max.y - min.y };
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	int segments = static_cast<int>(gradient->Size()) - 1;
	float x = min.x;
	for(size_t i = 0; i < segments; i++)
	{
		const Gradient::Key& curr = gradient->Keys()[i];
		const Gradient::Key& next = gradient->Keys()[i + 1];

		float segWidth = size.x * (next.t - curr.t);
		ImU32 left = static_cast<ImU32>(ImColor(curr.value.r, curr.value.g, curr.value.b, curr.value.a));
		ImU32 right = static_cast<ImU32>(ImColor(next.value.r, next.value.g, next.value.b, next.value.a));
		drawList->AddRectFilledMultiColor({ x, min.y }, { x + segWidth, min.y + size.y }, left, right, right, left);
		x += segWidth;
	}
}

static void DrawGradientPopup(Gradient* gradient)
{
	static const float markerSize = 10.0f;
	static const ImVec2 size = { 350.0f, 50.0f };
	static const char* editPopupId = "##gradientPopupColorEdit";

	static Color* editColor = nullptr;
	static int selected = -1;

	ImVec2 origin = ImGui::GetCursorScreenPos();
	ImVec2 end = { origin.x + size.x, origin.y + size.y };
	DrawGradient(origin, end, gradient);

	ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 mp = ImGui::GetMousePos();
	for(size_t i = 0; i < gradient->Size(); i++)
	{
		const Gradient::Key& key = gradient->At(i);
		ImVec2 pos = { origin.x + key.t * size.x, end.y };
		Vector2 center = Vector2(pos.x, pos.y + markerSize * 0.5f);
		bool hovered = Vector2::Distance(center, Vector2(mp.x, mp.y)) < markerSize;

		ImU32 color = hovered ? static_cast<ImU32>(static_cast<ImColor>(style.Colors[ImGuiCol_ButtonHovered])) : IM_COL32(255, 255, 255, 255);
		drawList->AddTriangleFilled(pos, { pos.x - markerSize * 0.5f, pos.y + markerSize }, { pos.x + markerSize * 0.5f, pos.y + markerSize }, color);

		if(hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			selected = i;
			editColor = &gradient->EditKey(i).value;
			ImGui::OpenPopup(editPopupId);
		}
	}
	ImGui::SetCursorScreenPos({ end.x, end.y + markerSize + ImGui::GetStyle().ItemSpacing.y });

	if(selected == -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		if(origin.x < mp.x && mp.x <= end.x && origin.y < mp.y && mp.y <= end.y + markerSize)
		{
			float t = Math::InverseLerp(origin.x, end.x, mp.x);
			gradient->AddKey(t, gradient->Evaluate(t));
		}
	}

	if(ImGui::BeginPopup(editPopupId))
	{
		if(editColor != nullptr)
		{
			ImGui::ColorPicker4("##colorPopupEditor", &editColor->r, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
			if(selected != 0 && selected != gradient->Size() - 1)
			{
				ImGui::Spacing();
				PushStyleColor(ImGuiCol_Button, EditorSettings::uiErrorColor, true);
				PushStyleColor(ImGuiCol_ButtonActive, EditorSettings::uiErrorColor, true);
				PushStyleColor(ImGuiCol_ButtonHovered, EditorSettings::uiErrorColor, true);
				bool result = ImGui::Button("Delete Key", { ImGui::GetContentRegionAvail().x, 0 });
				ImGui::PopStyleColor(3);
				if(result)
				{
					gradient->RemoveKey(selected);
					ImGui::CloseCurrentPopup();
				}
			}
		}
		ImGui::EndPopup();
	}
	else
	{
		editColor = nullptr;
		selected = -1;
	}
}

void GuiHelper::GradientEdit(const char* label, Gradient* gradient)
{
	static const char* popupId = "Gradient##GradientEditorPopup";

	ImVec2 origin = ImGui::GetCursorScreenPos();
	ImVec2 size = ImGui::GetItemRectSize();
	ImVec2 end = { origin.x + size.x, origin.y + size.y };
	DrawGradient(origin, end, gradient);

	std::string btnLabel = std::string(label) + "_EditButton";
	PushStyleColor(ImGuiCol_Button, Color(0.0f, 0.0f, 0.0f, 0.0f), false);
	PushStyleColor(ImGuiCol_ButtonActive, EditorSettings::c500.WithAlpha(0.6f), false);
	PushStyleColor(ImGuiCol_ButtonHovered, EditorSettings::c500.WithAlpha(0.5f), false);
	if(ImGui::Button(btnLabel.c_str(), size))
	{
		ImGui::OpenPopup(popupId);
	}
	ImGui::PopStyleColor(3);

	CenterNextWindow();
	if(ImGui::BeginPopup(popupId, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DrawGradientPopup(gradient);
		ImGui::EndPopup();
	}
}
