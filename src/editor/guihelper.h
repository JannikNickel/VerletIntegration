#pragma once
#include "structs/color.h"
#include "structs/vector2.h"
#include "structs/gradient.h"
#include "utils/math.h"
#include "editorsettings.h"
#include "imgui.h"
#include "magic_enum.hpp"
#include <algorithm>
#include <limits>
#include <optional>

namespace GuiHelper
{
	inline void PushStyleColor(ImGuiCol col, const std::optional<Color>& color, bool copyAlpha = false)
	{
		const ImVec4& defCol = ImGui::GetStyle().Colors[col];
		ImGui::PushStyleColor(col, color.has_value() ? ImVec4 { color.value().r, color.value().g, color.value().b, copyAlpha ? defCol.w : color.value().a } : defCol);
	}

	inline void BeginDisabled(bool disabled)
	{
		if(disabled)
		{
			ImGui::BeginDisabled();
		}
	}

	inline void EndDisabled(bool disabled)
	{
		if(disabled)
		{
			ImGui::EndDisabled();
		}
	}

	inline float TitleBarHeight()
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		return ImGui::GetFontSize() + style.FramePadding.y * 2.0f;
	}

	inline void CenterNextWindow()
	{
		ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f }, ImGuiCond_Always, { 0.5f, 0.5f });
	}

	inline int HorizontalButton(const char* button, float fullWidth, const std::optional<Color>& color = std::nullopt, bool disabled = false)
	{
		PushStyleColor(ImGuiCol_Button, color, true);
		PushStyleColor(ImGuiCol_ButtonActive, color, true);
		PushStyleColor(ImGuiCol_ButtonHovered, color, true);
		if(disabled)
		{
			ImGui::BeginDisabled();
		}
		bool result = ImGui::Button(button, { fullWidth - ImGui::GetStyle().ItemInnerSpacing.x, 0 });
		if(disabled)
		{
			ImGui::EndDisabled();
		}
		ImGui::PopStyleColor(3);
		return result;
	}

	inline int HorizontalButtonSplit(const char* button0, const char* button1, const std::optional<Color>& b0Color = std::nullopt, const std::optional<Color>& b1Color = std::nullopt, bool b0Disabled = false, bool b1Disabled = false)
	{
		float width = ImGui::GetContentRegionAvail().x;
		if(HorizontalButton(button0, width * 0.5f, b0Color, b0Disabled))
		{
			return 1;
		}
		ImGui::SameLine();
		if(HorizontalButton(button1, width * 0.5f, b1Color, b1Disabled))
		{
			return 2;
		}
		return 0;
	}

	template<typename T> requires std::is_enum<T>::value
		inline bool EnumDropdown(const char* label, T* value)
	{
		bool changed = false;
		if(ImGui::BeginCombo(label, magic_enum::enum_name<T>(*value).data()))
		{
			for(T entry : magic_enum::enum_values<T>())
			{
				if(ImGui::Selectable(magic_enum::enum_name<T>(entry).data(), *value == entry))
				{
					*value = entry;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		return changed;
	}

	inline bool ClampedFloatInput(const char* label, float* value, const char* format = "%0.2f", float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max())
	{
		if(ImGui::InputFloat(label, value, 0.0f, 0.0f, format))
		{
			*value = std::clamp(*value, min, max);
			return true;
		}
		return false;
	}

	inline bool ClampedFloat2Input(const char* label, float value[2], const char* format = "%0.2f", float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max())
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

	inline void _DrawGradient(ImVec2 min, ImVec2 max, Gradient* gradient)
	{
		ImVec2 size = { max.x - min.x, max.y - min.y };
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		int parts = gradient->Keys().size() - 1;
		float x = min.x;
		for(size_t i = 0; i < parts; i++)
		{
			const Gradient::Key& curr = gradient->Keys()[i];
			const Gradient::Key& next = gradient->Keys()[i + 1];

			float nextT = next.t;
			float partW = size.x * (next.t - curr.t);
			ImU32 left = IM_COL32(curr.value.r * 255, curr.value.g * 255, curr.value.b * 255, curr.value.a * 255);
			ImU32 right = IM_COL32(next.value.r * 255, next.value.g * 255, next.value.b * 255, next.value.a * 255);
			drawList->AddRectFilledMultiColor({ x, min.y }, { x + partW, min.y + size.y }, left, right, right, left);
			x += partW;
		}
	}

	inline void _DrawGradientPopup(Gradient* gradient)
	{
		static const float markerSize = 10.0f;
		static Color* editColor = nullptr;
		static int selected = -1;

		ImVec2 origin = ImGui::GetCursorScreenPos();
		ImVec2 size = { 350.0f, 50.0f };
		ImVec2 end = { origin.x + size.x, origin.y + size.y };
		_DrawGradient(origin, end, gradient);

		ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 mp = ImGui::GetMousePos();
		int i = 0;
		for(const Gradient::Key& key : gradient->Keys())
		{
			ImVec2 pos = { origin.x + key.t * size.x, end.y };
			ImU32 color = IM_COL32(255, 255, 255, 255);
			Vector2 p = Vector2(pos.x, pos.y + markerSize * 0.5f);
			if(Vector2::Distance(p, Vector2(mp.x, mp.y)) < markerSize)
			{
				ImVec4 col = style.Colors[ImGuiCol_ButtonHovered];
				color = IM_COL32(col.x * 255, col.y * 255, col.z * 255, col.w * 255);
				if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					selected = i;
					editColor = &const_cast<Gradient::Key&>(key).value;
					ImGui::OpenPopup("##gradientPopupColorEdit");
				}
			}
			drawList->AddTriangleFilled(pos, { pos.x - markerSize * 0.5f, pos.y + markerSize }, { pos.x + markerSize * 0.5f, pos.y + markerSize }, color);
			i++;
		}

		if(selected == -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if(origin.x < mp.x && mp.x <= end.x && origin.y < mp.y && mp.y <= end.y + markerSize)
			{
				float t = Math::InverseLerp(origin.x, end.x, mp.x);
				gradient->AddKey(t, gradient->Evaluate(t));
			}
		}

		ImGui::SetCursorScreenPos({ end.x, end.y + markerSize + ImGui::GetStyle().ItemSpacing.y });

		if(ImGui::BeginPopup("##gradientPopupColorEdit"))
		{
			if(editColor != nullptr)
			{
				ImGui::ColorPicker4("##colorPopupEditor", &editColor->r, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
				if(selected != 0 && selected != gradient->Keys().size() - 1)
				{
					ImGui::Spacing();
					if(HorizontalButton("Delete Key", ImGui::GetContentRegionAvail().x + ImGui::GetStyle().ItemInnerSpacing.x, EditorSettings::uiErrorColor))
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

	inline bool GradientEdit(const char* label, Gradient* gradient)
	{
		ImVec2 origin = ImGui::GetCursorScreenPos();
		ImVec2 size = ImGui::GetItemRectSize();
		ImVec2 end = { origin.x + size.x, origin.y + size.y };
		_DrawGradient(origin, end, gradient);

		std::string btnLabel = std::string(label) + "_btn";
		if(ImGui::InvisibleButton(btnLabel.c_str(), size))
		{
			ImGui::OpenPopup("Gradient");
		}

		CenterNextWindow();
		if(ImGui::BeginPopup("Gradient", ImGuiWindowFlags_AlwaysAutoResize))
		{
			_DrawGradientPopup(gradient);
			ImGui::EndPopup();
		}

		return false;
	}
}
