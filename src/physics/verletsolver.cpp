#include "verletsolver.h"
#include "structs/vector2.h"
#include "simulation/simulation.h"
#include <cmath>
#include <exception>

VerletSolver::VerletSolver(EcsWorld& ecs, IConstraint* constraint, float timeStep, float gravity, unsigned int substeps) : ecs(ecs), constraint(constraint), timeStep(timeStep), gravity(gravity), substeps(substeps), collision(true), updateMode(SolverUpdateMode::FrameDeltaTime)
{

}

void VerletSolver::Update(float dt)
{
	switch(updateMode)
	{
		case SolverUpdateMode::FrameDeltaTime:
		{
			Simulate(dt);
			break;
		}
		case SolverUpdateMode::FrameFixedStep:
		{
			Simulate(timeStep);
			break;
		}
		case SolverUpdateMode::FixedFrameRate:
		{
			static float timer = 0.0f;
			timer += dt;
			while(timer >= timeStep)
			{
				Simulate(timeStep);
				timer -= timeStep;
			}
			break;
		}
		default:
			throw std::exception("[VerletSolver::Update] Missing switch case!");
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
		if(collision)
		{
			Collisions();
		}
		Move(stepDt);
	}
}

void VerletSolver::Gravity()
{
	ecs.QueryMT<PhysicsCircle>(std::nullopt, [this](PhysicsCircle& p)
	{
		p.acc.y += gravity;
	});
}

void VerletSolver::Constraint()
{
	ecs.QueryMT<Transform, PhysicsCircle>(std::nullopt, [this](Transform& t, PhysicsCircle& p)
	{
		constraint->Contrain(t.Position(), p);
	});
}

void VerletSolver::Collisions()
{
	ecs.QueryPairs<Transform, PhysicsCircle>([this](Transform& aTransform, PhysicsCircle& a, Transform& bTransform, PhysicsCircle& b)
	{
		Vector2& aPos = aTransform.Position();
		Vector2& bPos = bTransform.Position();
		Vector2 dir = aPos - bPos;
		float dst = dir.Length();
		if(dst < a.radius + b.radius)
		{
			Vector2 normDir = dir / dst;
			float overlap = a.radius + b.radius - dst;
			float massRatio = b.mass / (a.mass + b.mass);
			aPos += normDir * (overlap * massRatio);
			bPos -= normDir * (overlap * (1.0f - massRatio));
		}
	});
}

void VerletSolver::Move(float dt)
{
	ecs.QueryMT<Transform, PhysicsCircle>(std::nullopt, [dt](Transform& t, PhysicsCircle& p)
	{
		Vector2& pos = t.Position();
		const Vector2 vel = pos - p.prevPos;
		p.prevPos = pos;
		pos += vel + p.acc * (dt * dt);
		p.acc = Vector2::zero;
	});
}
