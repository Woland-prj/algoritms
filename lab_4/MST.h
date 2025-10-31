#ifndef MST_H
#define MST_H

#include "Structs.h"

// mst - Minimal Spanning Tree (работа с минимальными остовыми деревьями)
namespace mst
{

// dsu - Disjoint Set Union (система неперсекающихся множеств для алгоритма Краскала)
class DSU final
{
public:
	explicit DSU(int n);

	int Find(int v);
	void Unite(int a, int b);

private:
	std::vector<int> m_parent, m_mark;
};

structs::WeightedGraph KruskalAlg(const structs::WeightedGraph& graph);

} // namespace mst

#endif // MST_H
