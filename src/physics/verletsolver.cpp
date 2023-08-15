#include "verletsolver.h"
#include "structs/vector2.h"
#include "simulation/simulation.h"
#include <cmath>
#include <exception>
#include <limits>

VerletSolver::VerletSolver(EcsWorld& ecs, IConstraint* constraint, float timeStep, float gravity, unsigned int substeps, float partitioningSize)
	: ecs(ecs), constraint(constraint), timeStep(timeStep), gravity(gravity), substeps(substeps), partitioningSize(partitioningSize),
	collision(true), updateMode(SolverUpdateMode::FrameDeltaTime), partitioning(constraint->Bounds().first, constraint->Bounds().second, partitioningSize)
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
			timer += std::fminf(dt, 0.25f);
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

void Solve(Transform& aTransform, PhysicsCircle& a, Transform& bTransform, PhysicsCircle& b)
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
}

void VerletSolver::Collisions()
{
	static int32_t cellsX = partitioning.CellsX();
	static int32_t cellsY = partitioning.CellsY();
	partitioning.Clear();

	ecs.QueryChunked<Transform, PhysicsCircle>(std::numeric_limits<size_t>::max(), [&](Transform* t, PhysicsCircle* p, size_t chunkSize)
	{
		for(size_t i = 0; i < chunkSize; i++)
		{
			partitioning.Insert(&t[i], &p[i]);
		}

		//This can be run for [1, lastXCell] (same for y), but the simulation is less stable in that case
		//Because the border cells dont check collisions within itself then
		//The same happens when trying to iterate with += 2 to prevent 2 cell pairs from being calculated twice
		for(int32_t i = 0; i < cellsX; i++)
		{
			for(int32_t k = 0; k < cellsY; k++)
			{
				PartitioningCell& cell = partitioning.At(i, k);
				for(int32_t x = i - 1; x < i + 2; x++)
				{
					for(int32_t y = k - 1; y < k + 2; y++)
					{
						if(x < 0 || x > cellsX - 1 || y < 0 || y > cellsY - 1)
						{
							continue;
						}
						PartitioningCell& other = partitioning.At(x, y);
						for(PartitioningCellEntry& a : cell)
						{
							for(PartitioningCellEntry& b : other)
							{
								if(a.t == b.t)
								{
									continue;
								}
								Solve(*a.t, *a.p, *b.t, *b.p);
							}
						}
					}
				}
			}
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
