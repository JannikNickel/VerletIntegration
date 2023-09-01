#include "vector2.h"
#include <cmath>
#include <algorithm>
#include <numbers>
#include <stdexcept>

const Vector2 Vector2::zero = Vector2(0.0f, 0.0f);
const Vector2 Vector2::one = Vector2(1.0f, 1.0f);

Vector2::Vector2(float x, float y) : x(x), y(y)
{

}

Vector2::Vector2() : x(0.0f), y(0.0f)
{

}

float Vector2::Length() const
{
	return std::sqrtf(x * x + y * y);
}

float Vector2::SqrLength() const
{
	return x * x + y * y;
}

float Vector2::Normalize()
{
	float length = Length();
	if(length > 0.0f)
	{
		x /= length;
		y /= length;
	}
	return length;
}

Vector2 Vector2::Normalized() const
{
	float length = Length();
	if(length > 0.0f)
	{
		return Vector2(x / length, y / length);
	}
	return *this;
}

Vector2 Vector2::PerpCW() const
{
	return Vector2(y, -x);
}

Vector2 Vector2::PerpCCW() const
{
	return Vector2(-y, x);
}

float Vector2::Distance(Vector2 a, Vector2 b)
{
	return (b - a).Length();
}

Vector2 Vector2::Lerp(Vector2 a, Vector2 b, float t)
{
	return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
}

Vector2 Vector2::LerpUnclamped(Vector2 a, Vector2 b, float t)
{
	return Vector2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

Vector2 Vector2::Slerp(Vector2 start, Vector2 target, float t)
{
	t = std::clamp(t, 0.0f, 1.0f);
	float dot = Dot(start, target);
	dot = std::clamp(dot, -1.0f, 1.0f);
	float theta = std::acosf(dot) * t;
	Vector2 direction = (target - start * dot).Normalized();
	return ((start * std::cos(theta)) + (direction * std::sin(theta)));
}

float Vector2::Dot(Vector2 a, Vector2 b)
{
	return a.x * b.x + a.y * b.y;
}

Vector2 Vector2::MoveTowards(Vector2 current, Vector2 target, float maxDelta)
{
	Vector2 dir = target - current;
	float length = dir.Length();
	if(length <= maxDelta || length <= 0.0f)
	{
		return target;
	}
	return current + dir / length * maxDelta;
}

float Vector2::Angle(Vector2 a, Vector2 b)
{
	return std::acosf(std::clamp(Dot(a.Normalized(), b.Normalized()), -1.0f, 1.0f)) * (180.0f / std::numbers::pi);
}

Vector2 Vector2::ClampLength(Vector2 v, float length)
{
	if(v.SqrLength() > length * length)
	{
		return v.Normalized() * length;
	}
	return v;
}

Vector2 Vector2::Min(Vector2 a, Vector2 b)
{
	return Vector2(std::min(a.x, b.x), std::min(a.y, b.y));
}

Vector2 Vector2::Max(Vector2 a, Vector2 b)
{
	return Vector2(std::max(a.x, b.x), std::max(a.y, b.y));
}

Vector2 Vector2::Rotate(Vector2 v, float angle)
{
	angle *= 0.0174533f;
	float s = std::sinf(angle);
	float c = std::cosf(angle);
	return Vector2(c * v.x - s * v.y, s * v.x + c * v.y);
}

bool Vector2::operator==(const Vector2& v) const
{
	return x == v.x && y == v.y;
}

bool Vector2::operator!=(const Vector2& v) const
{
	return !(*this == v);
}

Vector2 Vector2::operator*(const float& s) const
{
	return Vector2(x * s, y * s);
}

void Vector2::operator*=(const float& s)
{
	x *= s;
	y *= s;
}

Vector2 Vector2::operator/(const float& s) const
{
	return Vector2(x / s, y / s);
}

void Vector2::operator/=(const float& s)
{
	x /= s;
	y /= s;
}

Vector2 Vector2::operator+(const Vector2& v) const
{
	return Vector2(x + v.x, y + v.y);
}

void Vector2::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;
}

Vector2 Vector2::operator-(const Vector2& v) const
{
	return Vector2(x - v.x, y - v.y);
}

void Vector2::operator-=(const Vector2& v)
{
	x -= v.x;
	y -= v.y;
}

float& Vector2::operator[](const int index)
{
	switch(index)
	{
		case 0:
			return x;
		case 1:
			return y;
	}
	throw std::out_of_range("Expected index [0, 2)");
}

float Vector2::operator[](const int index) const
{
	switch(index)
	{
		case 0:
			return x;
		case 1:
			return y;
	}
	throw std::out_of_range("Expected index [0, 2)");
}

Vector2 Vector2::operator-()
{
	return Vector2(-x, -y);
}

std::ostream& operator<<(std::ostream& os, const Vector2& v)
{
	return (os << "[Vector2] (" << v.x << ", " << v.y << ")");
}
