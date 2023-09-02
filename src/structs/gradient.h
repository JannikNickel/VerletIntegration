#pragma once
#include "color.h"
#include <vector>
#include <initializer_list>
#include <cstdint>

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

	size_t Size() const;
	const std::vector<Key>& Keys() const;
	const Key& At(size_t index) const;
	Key& EditKey(size_t index);

	void AddKey(float time, const Color& value);
	void AddKey(const Key& key);
	void RemoveKey(int index);
	Color Evaluate(float time) const;

private:
	std::vector<Key> keys = {};
};
