#pragma once
#include "imgui.h"
#include "magic_enum.hpp"

namespace GuiHelper
{
	inline float TitleBarHeight()
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		return ImGui::GetFontSize() + style.FramePadding.y * 2.0f;
	}

	inline void CenterNextWindow()
	{
		ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f }, ImGuiCond_Always, { 0.5f, 0.5f });
	}

	inline int HorizontalButtonSplit(const char* button0, const char* button1)
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		float width = ImGui::GetContentRegionAvail().x;
		if(ImGui::Button("Create", { width * 0.5f - style.ItemInnerSpacing.x, 0 }))
		{
			return 1;
		}
		ImGui::SameLine();
		if(ImGui::Button("Cancel", { width * 0.5f - style.ItemInnerSpacing.x, 0 }))
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
}
