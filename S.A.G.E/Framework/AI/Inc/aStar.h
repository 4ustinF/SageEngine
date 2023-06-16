#pragma once

#include "GridBasedGraph.h"

namespace AI
{
	using NodeList = std::list<GridBasedGraph::Node*>;
	using GetCost = std::function<float(const GridBasedGraph::Node*, const GridBasedGraph::Node*)>;
	using GetHeuristics = std::function<float(const GridBasedGraph::Node*, const GridBasedGraph::Node*)>;

	class aStar
	{
	public:
		bool Run(GridBasedGraph& graph, int startX, int startY, int endX, int endY, GetCost getCost, GetHeuristics getHeur);

		const NodeList& GetClosedList() const { return mClosedList; }

		~aStar()
		{
			mOpenList.clear();
			mClosedList.clear();
		}

	private:
		NodeList mOpenList;
		NodeList mClosedList;

		void SortInsertOpenList(AI::GridBasedGraph::Node* node);
	};
}