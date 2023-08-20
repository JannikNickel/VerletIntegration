#include "verletsolver.h"
#include "structs/vector2.h"
#include "simulation/simulation.h"
#include <cmath>
#include <exception>
#include <limits>
#include <utility>
#include <array>

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

void VerletSolver::Collisions()
{
	static const std::array<std::pair<int32_t, int32_t>, 4> cellOffsets =
	{
		std::make_pair(1, 1),
		std::make_pair(1, 0),
		std::make_pair(1, -1),
		std::make_pair(0, -1)
	};
	static const int32_t cellsX = partitioning.CellsX();
	static const int32_t cellsY = partitioning.CellsY();
	static const int32_t lastXCell = cellsX - 1;
	static const int32_t lastYCell = cellsY - 1;

	ecs.QueryChunked<Transform, PhysicsCircle>(std::numeric_limits<size_t>::max(), [&](Transform* t, PhysicsCircle* p, size_t chunkSize)
	{
		partitioning.Clear();
		for(size_t i = 0; i < chunkSize; i++)
		{
			partitioning.Insert(&t[i], &p[i]);
		}

		for(int32_t i = 0; i < cellsX; i++)
		{
			for(int32_t k = 0; k < cellsY; k++)
			{
				PartitioningCell& cell = partitioning.At(i, k);
				SolveCell(cell);

				for(const auto& [xOff, yOff] : cellOffsets)
				{
					int32_t x = i + xOff;
					int32_t y = k + yOff;
					if(((static_cast<uint32_t>(x) > lastXCell) | (static_cast<uint32_t>(y) > lastYCell)) != 0)
					{
						continue;
					}

					SolveCells(cell, partitioning.At(x, y));
				}
			}
		}
	});
}

void VerletSolver::SolveCell(PartitioningCell& cell)
{
	size_t count = cell.size();
	for(size_t i = 0; i < count; i++)
	{
		PartitioningCellEntry& a = cell[i];
		for(size_t k = i + 1; k < count; k++)
		{
			PartitioningCellEntry& b = cell[k];
			Solve(*a.t, *a.p, *b.t, *b.p);
		}
	}
}

void VerletSolver::SolveCells(PartitioningCell& cell0, PartitioningCell& cell1)
{
	for(PartitioningCellEntry& a : cell0)
	{
		for(PartitioningCellEntry& b : cell1)
		{
			Solve(*a.t, *a.p, *b.t, *b.p);
		}
	}
}

void VerletSolver::Solve(Transform& aTransform, PhysicsCircle& a, Transform& bTransform, PhysicsCircle& b)
{
	Vector2& aPos = aTransform.Position();
	Vector2& bPos = bTransform.Position();
	Vector2 dir = aPos - bPos;
	float dst = dir.SqrLength();
	float r = a.radius + b.radius;
	r *= r;
	if(dst < r)
	{
		dst = std::sqrtf(dst);
		Vector2 normDir = dir / dst;
		float overlap = a.radius + b.radius - dst;
		float massRatio = b.mass / (a.mass + b.mass);
		aPos += normDir * (overlap * massRatio);
		bPos -= normDir * (overlap * (1.0f - massRatio));
	}
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
