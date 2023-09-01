#pragma once
#include "color.h"
#include <vector>
#include <initializer_list>

struct Gradient
{
	struct Key
	{
		float t;
		Color value;

		Key(float t, const Color& value) : t(t), value(value) { }
	};

	static const Gradient white;

	Gradient(std::initializer_list<Key> keys = {});

	const std::vector<Key>& Keys() const;
	void AddKey(float time, const Color& value);
	void AddKey(const Key& key);
	void RemoveKey(int index);
	Color Evaluate(float time) const;

private:
	std::vector<Key> keys = {};
};
