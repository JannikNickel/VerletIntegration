#include "gradient.h"
#include "utils/math.h"

const Gradient Gradient::white = Gradient({ Gradient::Key(0.0f, Color(1.0f, 1.0f, 1.0f, 1.0f)), Gradient::Key(1.0f, Color(1.0f, 1.0f, 1.0f, 1.0f)) });

Gradient::Gradient(std::initializer_list<Key> keys)
{
	for(const Key& key : keys)
	{
		AddKey(key.t, key.value);
	}
}

size_t Gradient::Size() const
{
	return keys.size();
}

const std::vector<Gradient::Key>& Gradient::Keys() const
{
	return keys;
}

const Gradient::Key& Gradient::At(size_t index) const
{
	return keys[index];
}

Gradient::Key& Gradient::EditKey(size_t index)
{
	return keys[index];
}

void Gradient::AddKey(float time, const Color& value)
{
	AddKey(Key(time, value));
}

void Gradient::AddKey(const Key& key)
{
	int index = keys.size();
	for(size_t i = 0; i < keys.size(); i++)
	{
		if(key.t < keys[i].t)
		{
			index = i;
			break;
		}
	}
	keys.insert(keys.begin() + index, key);
}

void Gradient::RemoveKey(int index)
{
	keys.erase(keys.begin() + index);
}

Color Gradient::Evaluate(float time) const
{
	time = std::clamp(time, 0.0f, 1.0f);
	for(size_t i = 0; i < keys.size(); i++)
	{
		if(time <= keys[i].t)
		{
			if(i == 0)
			{
				return keys[i].value;
			}
			const Key& left = keys[i - 1];
			const Key& right = keys[i];
			float t = Math::InverseLerp(left.t, right.t, time);
			return Color::Lerp(left.value, right.value, t);
		}
	}
	return keys.size() > 0 ? keys.back().value : Color::black;
}
