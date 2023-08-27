#pragma once

namespace Math
{
	inline int Sgn(float value)
	{
		return (0.0f < value) - (value < 0.0f);
	}
};
