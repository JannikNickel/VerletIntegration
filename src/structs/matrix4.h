#pragma once
#include "structs/vector2.h"

struct Matrix4
{
    float cells[16];

    static const Matrix4 identity;

    Matrix4();
    Matrix4(float c0, float c1, float c2, float c3, float c4, float c5, float c6, float c7, float c8, float c9, float c10, float c11, float c12, float c13, float c14, float c15);

    Matrix4& Mult2d(const Matrix4& m);

    static Matrix4 PositionScale2d(Vector2 pos, Vector2 scale);
    static Matrix4 PositionScale2d(Vector2 pos, float scale);
    static Matrix4 Pos(Vector2 pos);
    static Matrix4 Scale(float scale);
    static Matrix4 RotZ(float radians);
    static Matrix4 Ortho(float left, float right, float bottom, float top, float near = 0.0f, float far = 100.0f);
    static Matrix4 Line(Vector2 from, Vector2 to);

    Matrix4 operator*(const Matrix4& m) const;
};
