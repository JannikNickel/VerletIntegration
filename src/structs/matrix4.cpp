#include "matrix4.h"

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
