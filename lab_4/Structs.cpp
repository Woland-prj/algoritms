#include "Structs.h"
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <istream>

namespace structs
{

WeightedGraph::WeightedGraph(size_t vertexCount)
{
	m_adjacencyMatrix.assign(vertexCount, std::vector<Weight>(vertexCount, NO_EDGE));
	m_totalWeight = 0;
}

WeightedGraph::WeightedGraph(std::istream& graphFile)
{
	size_t edgeCount = 0;
	size_t vertexCount = 0;

	if (!(graphFile >> edgeCount >> vertexCount))
		throw std::invalid_argument("Ошибка чтения количества рёбер и вершин");

	m_adjacencyMatrix.assign(vertexCount, std::vector<Weight>(vertexCount, NO_EDGE));
	m_totalWeight = 0;

	for (size_t i = 0; i < edgeCount; ++i)
	{
		Vertex u, v;
		Weight w;

		if (!(graphFile >> u >> v >> w))
			throw std::invalid_argument("Ошибка чтения ребра");

		AddEdge(u, v, w);
	}
}

bool WeightedGraph::AddEdge(const Vertex u, const Vertex v, const Weight w)
{
	if (u > m_adjacencyMatrix.size() || v > m_adjacencyMatrix.size())
		throw std::out_of_range("Vertex index out of range");

	if (u == v)
		return false;

	if (w <= 0)
		return false;

	auto uI = u - 1;
	auto vI = v - 1;
	if (m_adjacencyMatrix[uI][vI] != NO_EDGE)
	{
		return false;
	}

	m_adjacencyMatrix[uI][vI] = w;
	m_adjacencyMatrix[vI][uI] = w;
	m_totalWeight += w;

	return true;
}

std::vector<WeightedGraphEdge> WeightedGraph::GetEdges() const
{
	std::vector<WeightedGraphEdge> edges;
	for (Vertex i = 0; i < m_adjacencyMatrix.size(); ++i)
	{
		for (Vertex j = i + 1; j < m_adjacencyMatrix.size(); ++j)
		{
			if (m_adjacencyMatrix[i][j] != NO_EDGE)
				edges.push_back({ static_cast<Vertex>(i + 1), static_cast<Vertex>(j + 1), m_adjacencyMatrix[i][j] });
		}
	}
	return edges;
}

Weight WeightedGraph::GetWeight() const
{
	return m_totalWeight;
}

std::ostream& operator<<(std::ostream& out, const WeightedGraph& graph)
{
	const auto& matrix = graph.m_adjacencyMatrix;
	out << "Adjacency matrix (" << matrix.size() << "x" << matrix.size() << "):\n";

	for (const auto& row : matrix)
	{
		for (auto w : row)
			out << std::setw(3) << w << " ";
		out << "\n";
	}

	out << "Total weight: " << graph.m_totalWeight << std::endl;
	return out;
}

static bool AreEdgesEqual(const WeightedGraphEdge& a, const WeightedGraphEdge& b)
{
	return (a.u == b.u && a.v == b.v) || (a.u == b.v && a.v == b.u);
}

std::vector<WeightedGraphEdge> operator-(
	const std::vector<WeightedGraphEdge>& left,
	const std::vector<WeightedGraphEdge>& right)
{
	std::vector<WeightedGraphEdge> result;

	for (const auto& eLeft : left)
	{
		bool found = std::any_of(right.begin(), right.end(),
			[&](const WeightedGraphEdge& eRight) {
				return AreEdgesEqual(eLeft, eRight);
			});

		if (!found)
			result.push_back(eLeft);
	}

	return result;
}

std::ostream& operator<<(std::ostream& out, const std::vector<WeightedGraphEdge>& eList)
{
	for (const auto& e : eList)
		out << e.u << " - " << e.v << " (" << e.w << ")\n";
	out.flush();
	return out;
}

} // namespace structs
