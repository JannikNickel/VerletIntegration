#include "matrix4.h"
#include <cmath>

const Matrix4 Matrix4::identity = 
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

Matrix4::Matrix4()
{
	*this = identity;
}

Matrix4::Matrix4(float c0, float c1, float c2, float c3, float c4, float c5, float c6, float c7, float c8, float c9, float c10, float c11, float c12, float c13, float c14, float c15) : cells { c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15 }
{

}

Matrix4& Matrix4::Mult2d(const Matrix4& m)
{
	Matrix4 t = *this;
	cells[0] = t.cells[0] * m.cells[0] + t.cells[1] * m.cells[4] + t.cells[3] * m.cells[12];
	cells[1] = t.cells[0] * m.cells[1] + t.cells[1] * m.cells[5] + t.cells[3] * m.cells[13];

	cells[4] = t.cells[4] * m.cells[0] + t.cells[5] * m.cells[4] + t.cells[7] * m.cells[12];
	cells[5] = t.cells[4] * m.cells[1] + t.cells[5] * m.cells[5] + t.cells[7] * m.cells[13];

	cells[12] = t.cells[12] * m.cells[0] + t.cells[13] * m.cells[4] + t.cells[15] * m.cells[12];
	cells[13] = t.cells[12] * m.cells[1] + t.cells[13] * m.cells[5] + t.cells[15] * m.cells[13];
	return *this;
}

Matrix4 Matrix4::PositionScale2d(Vector2 pos, Vector2 scale)
{
	return
	{
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		pos.x, pos.y, 0.0f, 1.0f
	};
}

Matrix4 Matrix4::PositionScale2d(Vector2 pos, float scale)
{
	return
	{
		scale, 0.0f, 0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		pos.x, pos.y, 0.0f, 1.0f
	};
}

Matrix4 Matrix4::Pos(Vector2 pos)
{
	return
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		pos.x, pos.y, 0.0f, 1.0f
	};
}

Matrix4 Matrix4::Scale(float scale)
{
	return
	{
		scale, 0.0f, 0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4 Matrix4::RotZ(float radians)
{
	float sin = std::sinf(radians);
	float cos = std::cosf(radians);
	return
	{
		cos, sin, 0.0f, 0.0f,
		-sin, cos, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4 Matrix4::Ortho(float left, float right, float bottom, float top, float near, float far)
{
	Matrix4 m = Matrix4::identity;
	m.cells[0] = 2.0f / (right - left);
	m.cells[5] = 2.0f / (top - bottom);
	m.cells[10] = -2.0f / (far - near);
	m.cells[12] = -((right + left) / (right - left));
	m.cells[13] = -((top + bottom) / (top - bottom));
	m.cells[14] = -((far + near) / (far - near));
	m.cells[4] = m.cells[8] = 0.0f;
	m.cells[1] = m.cells[9] = 0.0f;
	m.cells[2] = m.cells[6] = 0.0f;
	m.cells[3] = m.cells[7] = m.cells[11] = 0.0f;
	m.cells[15] = 1.0f;
	return m;
}

Matrix4 Matrix4::Line(Vector2 from, Vector2 to)
{
	Vector2 dir = (to - from);
	float len = dir.Normalize();
	return Matrix4::Scale(len).Mult2d(Matrix4::RotZ(atan2f(dir.y, dir.x))).Mult2d(Matrix4::Pos(from + dir * (len * 0.5f)));
}

Matrix4 Matrix4::operator*(const Matrix4& m) const
{
	Matrix4 res;
	res.cells[0] = cells[0] * m.cells[0] + cells[1] * m.cells[4] + cells[2] * m.cells[8] + cells[3] * m.cells[12];
	res.cells[1] = cells[0] * m.cells[1] + cells[1] * m.cells[5] + cells[2] * m.cells[9] + cells[3] * m.cells[13];
	res.cells[2] = cells[0] * m.cells[2] + cells[1] * m.cells[6] + cells[2] * m.cells[10] + cells[3] * m.cells[14];
	res.cells[3] = cells[0] * m.cells[3] + cells[1] * m.cells[7] + cells[2] * m.cells[11] + cells[3] * m.cells[15];

	res.cells[4] = cells[4] * m.cells[0] + cells[5] * m.cells[4] + cells[6] * m.cells[8] + cells[7] * m.cells[12];
	res.cells[5] = cells[4] * m.cells[1] + cells[5] * m.cells[5] + cells[6] * m.cells[9] + cells[7] * m.cells[13];
	res.cells[6] = cells[4] * m.cells[2] + cells[5] * m.cells[6] + cells[6] * m.cells[10] + cells[7] * m.cells[14];
	res.cells[7] = cells[4] * m.cells[3] + cells[5] * m.cells[7] + cells[6] * m.cells[11] + cells[7] * m.cells[15];

	res.cells[8] = cells[8] * m.cells[0] + cells[9] * m.cells[4] + cells[10] * m.cells[8] + cells[11] * m.cells[12];
	res.cells[9] = cells[8] * m.cells[1] + cells[9] * m.cells[5] + cells[10] * m.cells[9] + cells[11] * m.cells[13];
	res.cells[10] = cells[8] * m.cells[2] + cells[9] * m.cells[6] + cells[10] * m.cells[10] + cells[11] * m.cells[14];
	res.cells[11] = cells[8] * m.cells[3] + cells[9] * m.cells[7] + cells[10] * m.cells[11] + cells[11] * m.cells[15];

	res.cells[12] = cells[12] * m.cells[0] + cells[13] * m.cells[4] + cells[14] * m.cells[8] + cells[15] * m.cells[12];
	res.cells[13] = cells[12] * m.cells[1] + cells[13] * m.cells[5] + cells[14] * m.cells[9] + cells[15] * m.cells[13];
	res.cells[14] = cells[12] * m.cells[2] + cells[13] * m.cells[6] + cells[14] * m.cells[10] + cells[15] * m.cells[14];
	res.cells[15] = cells[12] * m.cells[3] + cells[13] * m.cells[7] + cells[14] * m.cells[11] + cells[15] * m.cells[15];
	return res;
}
