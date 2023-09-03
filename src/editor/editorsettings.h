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

	const Color c50  = Color::From32(245, 247, 250);
	const Color c100 = Color::From32(235, 239, 243);
	const Color c200 = Color::From32(211, 219, 228);
	const Color c300 = Color::From32(172, 189, 205);
	const Color c400 = Color::From32(126, 152, 178);
	const Color c500 = Color::From32(94, 124, 153);
	const Color c600 = Color::From32(74, 99, 127);
	const Color c700 = Color::From32(61, 80, 103);
	const Color c800 = Color::From32(53, 69, 87);
	const Color c900 = Color::From32(48, 59, 74);
	const Color c950 = Color::From32(22, 27, 34);

	void ApplyUIStyle();
}
