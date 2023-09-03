#pragma once
#include <cstring>

#define NAMEOF(arg) _nameof(#arg)

inline constexpr const char* _nameof(const char* arg)
{
	const char* separator = nullptr;
	const char* pos = arg;
	while(*pos != '\0')
	{
		if(*pos == ':' || *pos == '.')
		{
			separator = pos;
		}
		pos++;
	}
	return separator != nullptr ? separator + 1 : arg;
}
