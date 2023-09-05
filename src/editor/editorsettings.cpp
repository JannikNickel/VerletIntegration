#include "editorsettings.h"
#include "imgui.h"

void ApplyColor(ImGuiCol setting, const Color color, bool copyAlpha = true)
{
	ImVec4* ref = &ImGui::GetStyle().Colors[setting];
	*ref = ImVec4(color.r, color.g, color.b, copyAlpha ? ref->w : color.a);
}

void EditorSettings::ApplyUIStyle()
{
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding.y = 4.0f;
	style.FrameRounding = 2.0f;
	style.WindowRounding = 2.0f;
	style.ScrollbarRounding = 2.0f;
	style.PopupRounding = 2.0f;
	style.ChildRounding = 2.0f;
	style.GrabRounding = 2.0f;
	style.TabRounding = 2.0f;

	ImVec4* colors = style.Colors;
	ApplyColor(ImGuiCol_Text, c50);
	ApplyColor(ImGuiCol_TextDisabled, c400);
	ApplyColor(ImGuiCol_WindowBg, c950);
	ApplyColor(ImGuiCol_ChildBg, c950);
	ApplyColor(ImGuiCol_PopupBg, c950);
	ApplyColor(ImGuiCol_Border, c800);
	ApplyColor(ImGuiCol_BorderShadow, c800);
	ApplyColor(ImGuiCol_FrameBg, c800);
	ApplyColor(ImGuiCol_FrameBgHovered, c600);
	ApplyColor(ImGuiCol_FrameBgActive, c500);
	ApplyColor(ImGuiCol_TitleBg, c950);
	ApplyColor(ImGuiCol_TitleBgActive, c700);
	ApplyColor(ImGuiCol_TitleBgCollapsed, c700);
	ApplyColor(ImGuiCol_MenuBarBg, c950);
	ApplyColor(ImGuiCol_ScrollbarBg, c950);
	ApplyColor(ImGuiCol_ScrollbarGrab, c800);
	ApplyColor(ImGuiCol_ScrollbarGrabHovered, c700);
	ApplyColor(ImGuiCol_ScrollbarGrabActive, c600);
	ApplyColor(ImGuiCol_CheckMark, c300);
	ApplyColor(ImGuiCol_SliderGrab, c500);
	ApplyColor(ImGuiCol_SliderGrabActive, c400);
	ApplyColor(ImGuiCol_Button, c500);
	ApplyColor(ImGuiCol_ButtonHovered, c500);
	ApplyColor(ImGuiCol_ButtonActive, c400);
	ApplyColor(ImGuiCol_Header, c400);
	ApplyColor(ImGuiCol_HeaderHovered, c500);
	ApplyColor(ImGuiCol_HeaderActive, c500);
	ApplyColor(ImGuiCol_Separator, c800);
	ApplyColor(ImGuiCol_SeparatorHovered, c800);
	ApplyColor(ImGuiCol_SeparatorActive, c800);
	ApplyColor(ImGuiCol_TextSelectedBg, c400);
	ApplyColor(ImGuiCol_ModalWindowDimBg, c900);
}
