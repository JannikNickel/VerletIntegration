#pragma once
#include <iostream>

struct Vector2
{
	float x;
	float y;

	static const Vector2 zero;
	static const Vector2 one;

	Vector2(float x, float y);
	Vector2();

	float Length() const;
	float SqrLength() const;
	float Normalize();
	Vector2 Normalized() const;
	Vector2 PerpCW() const;
	Vector2 PerpCCW() const;

	static float Distance(Vector2 a, Vector2 b);
	static Vector2 Lerp(Vector2 a, Vector2 b, float t);
	static Vector2 LerpUnclamped(Vector2 a, Vector2 b, float t);
	static Vector2 Slerp(Vector2 start, Vector2 target, float t);
	static float Dot(Vector2 a, Vector2 b);
	static Vector2 MoveTowards(Vector2 current, Vector2 target, float maxDelta);
	static float Angle(Vector2 a, Vector2 b);
	static Vector2 ClampLength(Vector2 v, float length);
	static Vector2 Min(Vector2 a, Vector2 b);
	static Vector2 Max(Vector2 a, Vector2 b);
	static Vector2 Rotate(Vector2 v, float angle);

	bool operator==(const Vector2& v) const;
	bool operator!=(const Vector2& v) const;

	Vector2 operator*(const float& s) const;
	void operator*=(const float& s);

	Vector2 operator/(const float& s) const;
	void operator/=(const float& s);

	Vector2 operator+(const Vector2& v) const;
	void operator+=(const Vector2& v);

	Vector2 operator-(const Vector2& v) const;
	void operator-=(const Vector2& v);

	float& operator[](const int index);
	float operator[](const int index) const;

	Vector2 operator-();
};

std::ostream& operator<<(std::ostream& os, const Vector2& v);