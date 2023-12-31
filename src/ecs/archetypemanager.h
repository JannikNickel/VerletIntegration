#pragma once
#include "archetype.h"
#include "threadpool.h"
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>
#include <ranges>
#include <memory>
#include <functional>
#include <optional>
#include <algorithm>

class ArchetypeManager
{
	friend class EcsWorld;

	struct ArchetypeIdHash
	{
		size_t operator()(const ArchetypeId& id) const
		{
			std::hash<int> hasher = {};
			size_t seed = 277;
			for(const ComponentId i : id)
			{
				seed = seed * 47 + hasher(i);
			}
			return seed;
		}
	};

	std::unordered_map<ArchetypeId, std::shared_ptr<Archetype>, ArchetypeIdHash> archetypes = {};
	std::unordered_map<ComponentId, std::vector<std::pair<ArchetypeId, size_t>>> componentArchetypeMap = {};

	//comps should be sorted based on componentId in ascending order!
	template<size_t N>
	std::shared_ptr<Archetype> ArchetypeFromComponents(const std::array<std::tuple<ComponentId, size_t, void*>, N>& comps)
	{
		auto idView = std::views::all(comps) | std::views::transform([](const std::tuple<ComponentId, size_t, void*>& comp) { return std::get<0>(comp); });
		ArchetypeId id = ArchetypeId(idView.begin(), idView.end());

		auto it = archetypes.find(id);
		if(it != archetypes.end())
		{
			return it->second;
		}

		std::shared_ptr<Archetype> archetype = std::make_shared<Archetype>(id);
		for(size_t i = 0; i < N; i++)
		{
			const std::tuple<ComponentId, size_t, void*>& comp = comps[i];
			archetype->components.push_back(Archetype::ComponentData { std::get<1>(comp), {} });

			auto [cIt, _] = componentArchetypeMap.emplace(std::get<0>(comp), std::vector<std::pair<ArchetypeId, size_t>>());
			cIt->second.push_back(std::make_pair(id, i));
		}
		archetypes.insert(std::make_pair(id, archetype));
		return archetype;
	}

	template<typename... Components, size_t... Indices>
	static std::tuple<Components*...> UnwrapCompPtr(Archetype::ComponentData** compData, std::index_sequence<Indices...>)
	{
		return std::make_tuple(reinterpret_cast<Components*>(&compData[Indices]->data[0])...);
	}

	template<typename Func, typename... Components, size_t... Indices>
	static void InvokeEntityFunc(Func&& entityFunc, std::tuple<Components*...>& comps, std::index_sequence<Indices...>)
	{
		entityFunc(*std::get<Indices>(comps)...);
	}

	template<typename... Components>
	std::vector<std::pair<ArchetypeId, std::array<size_t, sizeof...(Components)>>> QueryArchetypes()
	{
		constexpr size_t numComps = sizeof...(Components);
		std::array<ComponentId, numComps> comps = { Components::componentId... };

		std::vector<std::pair<ArchetypeId, std::array<size_t, numComps>>> archs = {};
		for(const std::pair<ArchetypeId, size_t>& pair : componentArchetypeMap[comps[0]])
		{
			std::array<size_t, numComps> arr = { pair.second };
			archs.push_back(std::make_pair(pair.first, arr));
		}
		for(size_t i = 1; i < numComps; i++)
		{
			const std::vector<std::pair<ArchetypeId, size_t>>& cArchs = componentArchetypeMap[comps[i]];
			for(int k = archs.size() - 1; k >= 0; k--)
			{
				std::pair<ArchetypeId, std::array<size_t, numComps>>& arch = archs[k];
				auto it = std::ranges::find_if(cArchs, [&](const std::pair<ArchetypeId, size_t>& pair) { return pair.first == arch.first; });
				if(it == cArchs.end())
				{
					archs.erase(archs.begin() + k);
					continue;
				}
				arch.second[i] = it->second;
			}
		}

		return archs;
	}

	template<typename... Components>
	std::pair<std::tuple<Components*...>, size_t> QueryComponentData(ArchetypeId archId, std::array<size_t, sizeof...(Components)> compIndices)
	{
		constexpr size_t numComps = sizeof...(Components);
		std::shared_ptr<Archetype> archetype = archetypes[archId];
		std::array<Archetype::ComponentData*, numComps> compData = {};
		for(size_t i = 0; i < numComps; i++)
		{
			compData[i] = &archetype->components[compIndices[i]];
		}
		std::tuple<Components*...> comps = UnwrapCompPtr<Components...>(compData.data(), std::make_index_sequence<numComps>{});
		return std::make_pair(comps, compData[0]->data.size() / compData[0]->elementSize);
	}

	template<typename Func, typename... Components>
	void QueryComponents(Func&& entityFunc)
	{
		constexpr size_t numComps = sizeof...(Components);
		std::vector<std::pair<ArchetypeId, std::array<size_t, numComps>>> archs = QueryArchetypes<Components...>();

		for(auto& [id, indices] : archs)
		{
			auto [comps, compCount] = QueryComponentData<Components...>(id, indices);
			for(size_t i = 0; i < compCount; i++)
			{
				InvokeEntityFunc(entityFunc, comps, std::make_index_sequence<numComps>{});
				((std::get<Components*>(comps)++), ...);
			}
		}
	}

	template<typename Func, typename... Components>
	void QueryComponentsMT(Func&& entityFunc, ThreadPool& threadPool, const std::optional<uint32_t>& threads = std::nullopt)
	{
		uint32_t threadCount = std::clamp(threads.value_or(std::thread::hardware_concurrency()), 1U, threadPool.ThreadCount());

		constexpr size_t numComps = sizeof...(Components);
		std::vector<std::pair<ArchetypeId, std::array<size_t, numComps>>> archs = QueryArchetypes<Components...>();

		for(auto& [id, indices] : archs)
		{
			auto [comps, compCount] = QueryComponentData<Components...>(id, indices);

			std::vector<std::pair<size_t, size_t>> sections = ThreadPool::SplitWork(compCount, threadCount);
			std::vector<std::thread> handles = {};
			for(size_t i = 0; i < threadCount; i++)
			{
				auto [offset, amount] = sections[i];

				if(amount == 0)
				{
					continue;
				}

				std::tuple<Components*...> tPtrs = comps;
				((std::get<Components*>(tPtrs) += offset), ...);

				auto workerFunc = [entityFunc, tPtrs = std::move(tPtrs), amount]() mutable
				{
					for(size_t i = 0; i < amount; i++)
					{
						InvokeEntityFunc(entityFunc, tPtrs, std::make_index_sequence<numComps>{});
						((std::get<Components*>(tPtrs)++), ...);
					}
				};
				threadPool.EnqueueJob(std::move(workerFunc));
			}
			threadPool.WaitForCompletion();
		}
	}

	template<typename Func, typename... Components>
	void QueryComponentsChunked(Func&& entityFunc, size_t chunkSize)
	{
		constexpr size_t numComps = sizeof...(Components);
		std::vector<std::pair<ArchetypeId, std::array<size_t, numComps>>> archs = QueryArchetypes<Components...>();

		for(auto& [id, indices] : archs)
		{
			auto [comps, compCount] = QueryComponentData<Components...>(id, indices);
			for(size_t i = 0; i < compCount; i += chunkSize)
			{
				size_t currentChunkSize = std::min(compCount - i, chunkSize);
				std::apply(entityFunc, std::tuple_cat(comps, std::make_tuple(currentChunkSize)));

				((std::get<Components*>(comps) += chunkSize), ...);
			}
		}
	}

	template<size_t START, size_t COUNT, typename... Components, size_t... Indices>
	static constexpr void IncCompPtrSeq(std::tuple<Components*...>& comps, std::ptrdiff_t inc, std::index_sequence<Indices...>)
	{
		((std::get<START + Indices>(comps) += inc), ...);
	}

	template<size_t START, size_t COUNT, typename... Components>
	static constexpr void IncCompPtrs(std::tuple<Components*...>& comps, std::ptrdiff_t inc)
	{
		IncCompPtrSeq<START, COUNT>(comps, inc, std::make_index_sequence<COUNT>{});
	}

	template<typename Func, typename... Components>
	void QueryComponentPairs(Func&& entityFunc)
	{
		constexpr size_t numComps = sizeof...(Components);
		std::vector<std::pair<ArchetypeId, std::array<size_t, numComps>>> archs = QueryArchetypes<Components...>();

		for(auto& [id, indices] : archs)
		{
			auto [comps, compCount] = QueryComponentData<Components...>(id, indices);
			auto cComps = std::tuple_cat(comps, comps);

			for(size_t i = 0; i < compCount; i++)
			{
				IncCompPtrs<numComps, numComps>(cComps, i + 1);
				for(size_t k = i + 1; k < compCount; k++)
				{
					InvokeEntityFunc(entityFunc, cComps, std::make_index_sequence<numComps * 2>{});
					IncCompPtrs<numComps, numComps>(cComps, 1);
				}
				size_t diff = (i + 1) + (compCount - (i + 1));
				IncCompPtrs<numComps, numComps>(cComps, -static_cast<std::ptrdiff_t>(diff));
				IncCompPtrs<0, numComps>(cComps, 1);
			}
		}
	}
};
