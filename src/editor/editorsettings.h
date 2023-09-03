#pragma once
#include "structs/color.h"

namespace EditorSettings
{
	const Color previewValidColor = Color::From32(53, 132, 222, 128);
	const Color previewInvalidColor = Color::From32(222, 53, 53, 128);
	const Color sceneObjectColor = Color::From32(255, 255, 255, 200);
	const Color selectedColor = Color::From32(222, 152, 53, 255);
	const Color selectedHoverColor = Color::From32(222, 183, 111, 255);

	const Color uiErrorColor = Color::From32(255, 53, 53, 200);
	const Color uiSuccessColor = Color::From32(53, 222, 78, 200);

	const Color c50  = Color::From32(246, 247, 249, 255);
	const Color c100 = Color::From32(237, 238, 241, 255);
	const Color c200 = Color::From32(214, 218, 225, 255);
	const Color c300 = Color::From32(179, 187, 198, 255);
	const Color c400 = Color::From32(137, 151, 167, 255);
	const Color c500 = Color::From32(107, 122, 140, 255);
	const Color c600 = Color::From32(82, 94, 111, 255);
	const Color c700 = Color::From32(70, 80, 94, 255);
	const Color c800 = Color::From32(60, 68, 80, 255);
	const Color c900 = Color::From32(53, 59, 69, 255);
	const Color c950 = Color::From32(36, 39, 45, 255);

	void ApplyUIStyle();
}
