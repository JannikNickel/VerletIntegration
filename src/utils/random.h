#pragma once
#include <random>
#include <cstdint>

class Random
{
public:
	Random() : generator(std::random_device()()) { }

	float Value()
	{
		static std::uniform_real_distribution<float> dist { 0.0f, 1.0f };
		return dist(generator);
	}

	float Range(float min, float max)
	{
		if(min >= max)
		{
			return max;
		}
		std::uniform_real_distribution<float> dist { min, max };
		return dist(generator);
	}

	int32_t Range(int32_t min, int32_t max)
	{
		if(min >= max)
		{
			return max;
		}
		std::uniform_int_distribution<int32_t> dist { min, max };
		return dist(generator);
	}

private:
	std::mt19937 generator;
};
