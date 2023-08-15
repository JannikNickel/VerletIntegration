#include "verletsolver.h"
#include "structs/vector2.h"
#include "simulation/simulation.h"
#include <cmath>
#include <exception>
#include <limits>

struct CellValue
{
	Transform* t;
	PhysicsCircle* p;

	CellValue(Transform* t, PhysicsCircle* p) : t(t), p(p) { }
};

using Cell = std::vector<CellValue>;
using Column = std::vector<Cell>;
using Grid = std::vector<Column>;

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
	static const float cellSize = 25.0f;
	static auto [bMin, bMax] = constraint->Bounds();
	static Vector2 bSize = bMax - bMin;
	static int32_t cellsX = static_cast<int32_t>(std::ceilf((bMax.x - bMin.x) / cellSize));
	static int32_t cellsY = static_cast<int32_t>(std::ceilf((bMax.y - bMin.y) / cellSize));
	static int32_t lastXCell = cellsX - 1;
	static int32_t lastYCell = cellsY - 1;
	static Grid grid = Grid(cellsX);
	for(int32_t i = 0; i < cellsX; i++)
	{
		grid[i] = Column(cellsY);
	}
	ecs.QueryChunked<Transform, PhysicsCircle>(std::numeric_limits<size_t>::max(), [this](Transform* t, PhysicsCircle* p, size_t chunkSize)
	{
		for(int32_t i = 0; i < cellsX; i++)
		{
			for(int32_t k = 0; k < cellsY; k++)
			{
				grid[i][k].clear();
			}
		}

		for(size_t i = 0; i < chunkSize; i++)
		{
			Vector2& pos = t[i].Position();
			int32_t cx = static_cast<int32_t>((pos.x - bMin.x) / bSize.x * static_cast<float>(cellsX));
			int32_t cy = static_cast<int32_t>((pos.y - bMin.y) / bSize.y * static_cast<float>(cellsY));
			grid[std::clamp(cx, 0, lastXCell)][std::clamp(cy, 0, lastYCell)].push_back(CellValue(&t[i], &p[i]));
		}

		for(int32_t i = 0; i < cellsX; i++)
		{
			for(int32_t k = 0; k < cellsY; k++)
			{
				Cell& cell = grid[i][k];
				for(int32_t x = i - 1; x < i + 2; x++)
				{
					for(int32_t y = k - 1; y < k + 2; y++)
					{
						if(x < 0 || x > lastXCell || y < 0 || y > lastYCell)
						{
							continue;
						}
						Cell& other = grid[x][y];
						for(CellValue& a : cell)
						{
							for(CellValue& b : other)
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
