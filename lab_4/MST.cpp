#include "MST.h"
#include <numeric>
#include <queue>

namespace mst
{

DSU::DSU(const int n)
	: m_parent(n + 1)
	, m_mark(n + 1, 0)
{
	std::iota(m_parent.begin(), m_parent.end(), 0);
}

int DSU::Find(const int v)
{
	if (v == m_parent[v])
		return v;
	return m_parent[v] = Find(m_parent[v]);
}

void DSU::Unite(int a, int b)
{
	a = Find(a);
	b = Find(b);
	if (a != b)
	{
		if (m_mark[a] < m_mark[b])
			std::swap(a, b);
		m_parent[b] = a;
		if (m_mark[a] == m_mark[b])
			m_mark[a]++;
	}
}

structs::WeightedGraph KruskalAlg(const structs::WeightedGraph& graph)
{
	using namespace structs;

	auto edges = graph.GetEdges();

	// Приоритетная очередь по неубыванию весов
	auto minHeapOrder = [](const WeightedGraphEdge& a, const WeightedGraphEdge& b) {
		return a > b;
	};
	std::priority_queue<WeightedGraphEdge, std::vector<WeightedGraphEdge>, decltype(minHeapOrder)> pq(minHeapOrder);

	for (const auto& e : edges)
		pq.push(e);

	// Количество вершин
	Vertex maxVertex = 0;
	for (const auto& e : edges)
		maxVertex = std::max({ maxVertex, e.u, e.v });

	DSU dsu(maxVertex);
	WeightedGraph mst(maxVertex + 1);

	while (!pq.empty())
	{
		auto edge = pq.top();
		pq.pop();

		if (dsu.Find(edge.u) != dsu.Find(edge.v))
		{
			dsu.Unite(edge.u, edge.v);
			mst.AddEdge(edge.u, edge.v, edge.w);
		}
	}

	return mst;
}

} // namespace mst
