#pragma once
#include "keycode.h"
#include "engine/window.h"
#include "structs/vector2.h"

class Input
{
	friend Window;

public:
	static Vector2 GetMousePosition();
	static Vector2 GetMouseDelta();
	static bool KeyPressed(KeyCode code);
	static bool KeyHeld(KeyCode code);
	static bool KeyReleased(KeyCode code);

private:
	static void Init(struct GLFWwindow* window);
	static void Update(struct GLFWwindow* window);
};
