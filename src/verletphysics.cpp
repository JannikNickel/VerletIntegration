#include "verletphysics.h"
#include <cmath>

void VerletPhysics::Update(float dt)
{
	unsigned int steps = std::max(substeps, 1u);
	float stepDt = dt / static_cast<float>(steps);
	for(unsigned int i = 0; i < steps; i++)
	{
		Gravity();
		Constraint();
		//Collisions();
		Move(stepDt);
	}
}

void VerletPhysics::Gravity()
{
	for(VerletObj& obj : objects)
	{
		obj.acc.y += gravity;
	}
}

void VerletPhysics::Constraint()
{
	for(VerletObj& obj : objects)
	{
		Vector2 dir = obj.pos - center;
		float dst = dir.Length();
		if(dst + obj.radius > radius)
		{
			Vector2 normDir = dir / dst;
			obj.pos = center + normDir * (radius - obj.radius);
		}
	}
}

void VerletPhysics::Collisions()
{
	size_t objCount = objects.size();
	for(size_t i = 0; i < objCount; i++)
	{
		VerletObj& a = objects[i];
		for(size_t k = i + 1; k < objCount; k++)
		{
			VerletObj& b = objects[k];
			Vector2 dir = a.pos - b.pos;
			float dst = dir.Length();
			if(dst < a.radius + b.radius)
			{
				Vector2 normDir = dir / dst;
				float overlap = a.radius + b.radius - dst;
				float massRatio = b.mass / (a.mass + b.mass);
				a.pos += normDir * (overlap * massRatio);
				b.pos -= normDir * (overlap * (1.0f - massRatio));
			}
		}
	}
}

void VerletPhysics::Move(float dt)
{
	for(VerletObj& obj : objects)
	{
		const Vector2 vel = obj.pos - obj.prevPos;
		obj.prevPos = obj.pos;
		obj.pos += vel + obj.acc * (dt * dt);
		obj.acc = Vector2::zero;
	}
}
