#include "color.h"

const Color Color::white = Color(1.0f, 1.0f, 1.0f, 1.0f);
const Color Color::black = Color(0.0f, 0.0f, 0.0f, 1.0f);

Color::Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
{

}

Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
{

}

Color Color::operator*(const Color& other) const
{
    return Color(r * other.r, g * other.g, b * other.b, a * other.a);
}

Color Color::From32(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

Color Color::FromHSV(float h, float s, float v, float a)
{
    int i = (int)(h * 6.0f);
    float f = (h * 6.0f) - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));
    switch(i % 6)
    {
        case 0:
            return Color(v, t, p);
        case 1:
            return Color(q, v, p);
        case 2:
            return Color(p, v, t);
        case 3:
            return Color(p, q, v);
        case 4:
            return Color(t, p, v);
        case 5:
            return Color(v, p, q);
        default:
            return Color::black;
    }
}

std::ostream& operator<<(std::ostream& os, const Color& c)
{
    return (os << "[Color] (" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")");
}
