#pragma once
#include "imgui.h"
#include "structs/color.h"
#include "magic_enum.hpp"
#include <algorithm>
#include <limits>
#include <optional>

namespace GuiHelper
{
	inline void PushStyleColor(ImGuiCol col, const std::optional<Color>& color, bool copyAlpha = false)
	{
		const ImVec4& defCol = ImGui::GetStyle().Colors[col];
		ImGui::PushStyleColor(col, color.has_value() ? ImVec4 { color.value().r, color.value().g, color.value().b, copyAlpha ? defCol.w : color.value().a }	: defCol);
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

	inline int HorizontalButtonHalf(const char* button, float fullWidth, const std::optional<Color>& color = std::nullopt, bool disabled = false)
	{
		PushStyleColor(ImGuiCol_Button, color, true);
		PushStyleColor(ImGuiCol_ButtonActive, color, true);
		PushStyleColor(ImGuiCol_ButtonHovered, color, true);
		if(disabled)
		{
			ImGui::BeginDisabled();
		}
		bool result = ImGui::Button(button, { fullWidth * 0.5f - ImGui::GetStyle().ItemInnerSpacing.x, 0 });
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
		if(HorizontalButtonHalf(button0, width, b0Color, b0Disabled))
		{
			return 1;
		}
		ImGui::SameLine();
		if(HorizontalButtonHalf(button1, width, b1Color, b1Disabled))
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
}
