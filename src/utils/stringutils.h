#pragma once
#include <string>
#include <cctype>
#include <algorithm>

namespace StringUtils
{
	inline std::string Trim(const std::string& str)
	{
		static const auto isTrimChar = [](char c)
		{
			return c == ' ' || c == '\0';
		};
		auto start = std::find_if_not(str.begin(), str.end(), isTrimChar);
		auto end = std::find_if_not(str.rbegin(), str.rend(), isTrimChar).base();
		return start < end ? std::string(start, end) : (end - start != -static_cast<int64_t>(str.size()) ? str : "");
	}
}
