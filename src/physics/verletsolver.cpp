#include "verletsolver.h"
#include <cmath>

VerletSolver::VerletSolver(float timeStep, IConstraint* constraint, float gravity, unsigned int substeps) : timeStep(timeStep), constraint(constraint), gravity(gravity), substeps(substeps)
{

}

void VerletSolver::Update(float dt)
{
	static float timer = 0.0f;
	timer += dt;
	while(timer >= timeStep)
	{
		Simulate(timeStep);
		timer -= timeStep;
	}
}

void VerletSolver::Simulate(float dt)
{
	unsigned int steps = std::max(substeps, 1u);
	float stepDt = dt / static_cast<float>(steps);
	for(unsigned int i = 0; i < steps; i++)
	{
		Gravity();
		Constraint();
		Collisions();
		Move(stepDt);
	}
}

void VerletSolver::Gravity()
{
	for(VerletObj& obj : objects)
	{
		obj.acc.y += gravity;
	}
}

void VerletSolver::Constraint()
{
	for(VerletObj& obj : objects)
	{
		constraint->Contrain(obj);
	}
}

void VerletSolver::Collisions()
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

void VerletSolver::Move(float dt)
{
	for(VerletObj& obj : objects)
	{
		const Vector2 vel = obj.pos - obj.prevPos;
		obj.prevPos = obj.pos;
		obj.pos += vel + obj.acc * (dt * dt);
		obj.acc = Vector2::zero;
	}
}
