#pragma once
#include <iostream>

struct Color
{
	float r;
	float g;
	float b;
	float a;

	static const Color white;
	static const Color black;

	Color();
	Color(float r, float g, float b, float a = 1.0f);

	static Color From32(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	static Color FromHSV(float h, float s, float v, float a = 1.0f);
};

std::ostream& operator<<(std::ostream& os, const Color& c);
