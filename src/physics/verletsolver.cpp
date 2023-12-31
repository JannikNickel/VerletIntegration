#include "verletsolver.h"
#include "structs/vector2.h"
#include "simulation/components.h"
#include <cmath>
#include <exception>
#include <limits>
#include <utility>

inline Vector2 CalcMassRatio(Particle& a, Particle& b)
{
	float aMul = static_cast<float>(!a.pinned);
	float bMul = static_cast<float>(!b.pinned);
	float massSum = a.mass * aMul + b.mass * bMul;
	float invMassSum = massSum != 0.0f ? (1.0f / massSum) : 0.0f;
	aMul *= b.mass * invMassSum;
	bMul *= a.mass * invMassSum;
	return Vector2(aMul, bMul);
}

VerletSolver::VerletSolver(EcsWorld& ecs, IConstraint& constraint, const SolverSettings& settings)
	: ecs(ecs), constraint(constraint), timeStep(settings.timestep), gravity(settings.gravity), substeps(settings.substeps), partitioningSize(settings.partitioningSize),
	collision(settings.collision), updateMode(settings.updateMode), partitioning(constraint.Bounds().first, constraint.Bounds().second, settings.partitioningSize)
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

	CollectStats();
}

void VerletSolver::Simulate(float dt)
{
	unsigned int steps = std::max(substeps, 1u);
	float stepDt = dt / static_cast<float>(steps);
	for(unsigned int i = 0; i < steps; i++)
	{
		if(collision)
		{
			Collisions();
		}
		UpdateObjects(stepDt);
		UpdateLinks(stepDt);
	}
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
	const int32_t cellsX = partitioning.CellsX();
	const int32_t cellsY = partitioning.CellsY();
	const int32_t lastXCell = cellsX - 1;
	const int32_t lastYCell = cellsY - 1;

	ecs.QueryChunked<Transform, Particle>(std::numeric_limits<size_t>::max(), [&](Transform* t, Particle* p, size_t chunkSize)
	{
		broadPhaseCounter.BeginSubFrame();
		partitioning.Clear();
		for(size_t i = 0; i < chunkSize; i++)
		{
			partitioning.Insert(&t[i], &p[i]);
		}
		broadPhaseCounter.EndSubFrame();

		narrowPhaseCounter.BeginSubFrame();
		for(const auto& [offset, amount] : ThreadPool::SplitWork(cellsX, threadPool.ThreadCount()))
		{
			threadPool.EnqueueJob([this, offset, amount, cellsY, lastXCell, lastYCell]
			{
				for(int32_t i = offset; i < offset + amount; i++)
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
		threadPool.WaitForCompletion();
		narrowPhaseCounter.EndSubFrame();
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

void VerletSolver::Solve(Transform& aTransform, Particle& a, Transform& bTransform, Particle& b)
{
	Vector2& aPos = aTransform.Position();
	Vector2& bPos = bTransform.Position();
	Vector2 dir = aPos - bPos;
	float dst = std::max(dir.SqrLength(), 0.001f);
	float r = a.radius + b.radius;
	r *= r;
	if(dst < r)
	{
		dst = std::sqrtf(dst);
		Vector2 normDir = dir / dst;
		float overlap = a.radius + b.radius - dst;

		auto [aMul, bMul] = CalcMassRatio(a, b);
		aPos += normDir * (overlap * aMul);
		bPos -= normDir * (overlap * bMul);
	}
}

void VerletSolver::UpdateObjects(float dt)
{
	updatePhaseCounter.BeginSubFrame();
	ecs.QueryMT<Transform, Particle>(std::nullopt, [this, dt](Transform& t, Particle& p)
	{
		if(p.pinned)
		{
			return;
		}

		//Make sure applied forces (like initial) are represented as force over 1 second to make it delta time and substep independent
		p.acc *= (1.0f / dt);

		//Apply mass to forces
		p.acc /= p.mass;

		//Gravity
		p.acc.x += gravity.x;
		p.acc.y += gravity.y;

		//Constrain
		constraint.Contrain(t.Position(), p);

		//Update
		Vector2& pos = t.Position();
		const Vector2 vel = pos - p.prevPos;
		p.prevPos = pos;
		pos += vel + p.acc * (dt * dt);
		p.acc = Vector2::zero;
	});
	updatePhaseCounter.EndSubFrame();
}

void VerletSolver::UpdateLinks(float dt)
{
	linkPhaseCounter.BeginSubFrame();
	ecs.Query<Transform, Link>([this, dt](Transform& t, Link& l)
	{
		Vector2& aPos = ecs.GetComponent<Transform>(l.e0)->Position();
		Vector2& bPos = ecs.GetComponent<Transform>(l.e1)->Position();
		Particle& a = *ecs.GetComponent<Particle>(l.e0);
		Particle& b = *ecs.GetComponent<Particle>(l.e1);

		Vector2 dir = (bPos - aPos);
		float len = dir.Normalize();
		float off = len - l.distance;
		if((l.restrictMax && off > 0.0f) || (l.restrictMin && off < 0.0f))
		{
			auto [aMul, bMul] = CalcMassRatio(a, b);
			aPos += dir * (off * aMul);
			bPos -= dir * (off * bMul);
		}
		t.value = Matrix4::Line(aPos, bPos);
	});
	linkPhaseCounter.EndSubFrame();
}

void VerletSolver::CollectStats()
{
	broadPhaseCounter.EndFrame();
	narrowPhaseCounter.EndFrame();
	updatePhaseCounter.EndFrame();
	linkPhaseCounter.EndFrame();
}

const FrameCounter& VerletSolver::BroadPhaseCounter() const
{
	return broadPhaseCounter;
}

const FrameCounter& VerletSolver::NarrowPhaseCounter() const
{
	return narrowPhaseCounter;
}

const FrameCounter& VerletSolver::UpdatePhaseCounter() const
{
	return updatePhaseCounter;
}

const FrameCounter& VerletSolver::LinkPhaseCounter() const
{
	return linkPhaseCounter;
}
