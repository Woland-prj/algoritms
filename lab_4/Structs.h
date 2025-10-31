#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstdint>
#include <ostream>
#include <utility>
#include <vector>

namespace structs
{

using Vertex = uint16_t;
using Weight = int;

constexpr Weight NO_EDGE = 0;

struct GraphEdge
{
	Vertex u;
	Vertex v;
};

struct WeightedGraphEdge
{
	Vertex u;
	Vertex v;
	Weight w;

	bool operator>(const WeightedGraphEdge& other) const noexcept
	{
		return w > other.w;
	}
};

static bool AreEdgesEqual(WeightedGraphEdge& a, WeightedGraphEdge& b);

std::vector<WeightedGraphEdge> operator-(
	const std::vector<WeightedGraphEdge>& left,
	const std::vector<WeightedGraphEdge>& right);

std::ostream& operator<<(std::ostream& out, const std::vector<WeightedGraphEdge>& eList);

class WeightedGraph final
{
public:
	WeightedGraph() = delete;
	explicit WeightedGraph(std::istream& graphFile);
	explicit WeightedGraph(size_t vertexCount);

	bool AddEdge(Vertex u, Vertex v, Weight w);
	std::vector<WeightedGraphEdge> GetEdges() const;
	Weight GetWeight() const;

	friend std::ostream& operator<<(std::ostream& out, const WeightedGraph& graph);

	~WeightedGraph() = default;

private:
	std::vector<std::vector<Weight>> m_adjacencyMatrix{};
	Weight m_totalWeight{};
};

} // namespace structs

#endif // STRUCTS_H
