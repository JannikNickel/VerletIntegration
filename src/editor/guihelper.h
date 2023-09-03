#pragma once
#include "structs/color.h"
#include "structs/vector2.h"
#include "structs/gradient.h"
#include "magic_enum.hpp"
#include "imgui.h"
#include <limits>
#include <optional>
#include <string_view>

namespace GuiHelper
{
	const ImGuiColorEditFlags defaultColorEditFlags = ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_AlphaPreview;

	float TitleBarHeight();
	void CenterNextWindow();

	int HorizontalButton(const char* button, float fullWidth, const std::optional<Color>& color = std::nullopt, bool disabled = false);
	int HorizontalButtonSplit(const char* button0, const char* button1, const std::optional<Color>& b0Color = std::nullopt, const std::optional<Color>& b1Color = std::nullopt, bool b0Disabled = false, bool b1Disabled = false);
	
	bool ClampedFloatInput(const char* label, float* value, const char* format = "%0.2f", float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max());
	bool ClampedFloat2Input(const char* label, float value[2], const char* format = "%0.2f", float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<float>::max());

	void GradientEdit(const char* label, Gradient* gradient);

	template<typename T> requires std::is_enum<T>::value
	bool EnumDropdown(const char* label, T* value)
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
}
