#pragma once
#include "simulation/simulation.h"
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>

struct PartitioningCellEntry
{
	Transform* t;
	PhysicsCircle* p;

	PartitioningCellEntry(Transform* t, PhysicsCircle* p) : t(t), p(p) { }
};

using PartitioningCell = std::vector<PartitioningCellEntry>;

class PartitioningGrid
{
public:
	PartitioningGrid(Vector2 min, Vector2 max, float cellSize) : bMin(min), bMax(max), bSize(max - min), cellSize(cellSize)
	{
		cellsX = static_cast<int32_t>(std::ceilf(bSize.x / cellSize));
		cellsY = static_cast<int32_t>(std::ceilf(bSize.y / cellSize));
		lastXCell = cellsX - 1;
		lastYCell = cellsY - 1;
		cells = std::vector<PartitioningCell>(cellsX * cellsY);
		for(PartitioningCell& cell : cells)
		{
			cell.reserve(16);
		}
	}

	void Clear()
	{
		for(PartitioningCell& cell : cells)
		{
			cell.clear();
		}
	}

	PartitioningCell& At(int32_t x, int32_t y)
	{
		return cells[y * cellsX + x];
	}

	void Insert(Transform* t, PhysicsCircle* p)
	{
		Vector2& pos = t->Position();
		int32_t cx = static_cast<int32_t>((pos.x - bMin.x) / bSize.x * static_cast<float>(cellsX));
		int32_t cy = static_cast<int32_t>((pos.y - bMin.y) / bSize.y * static_cast<float>(cellsY));
		cells[std::clamp(cy, 0, lastYCell) * cellsX + std::clamp(cx, 0, lastXCell)].push_back(PartitioningCellEntry(t, p));
	}

	int32_t CellsX() const { return cellsX; }
	int32_t CellsY() const { return cellsY; }

private:
	std::vector<PartitioningCell> cells;
	const Vector2 bMin;
	const Vector2 bMax;
	const Vector2 bSize;
	const float cellSize;
	int32_t cellsX;
	int32_t cellsY;
	int32_t lastXCell;
	int32_t lastYCell;
};
