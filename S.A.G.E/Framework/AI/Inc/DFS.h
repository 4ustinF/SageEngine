#pragma once

#include "GridBasedGraph.h"

namespace AI
{
	using NodeList = std::list<GridBasedGraph::Node*>;

	class DFS
	{
	public:
		bool Run(GridBasedGraph& graph, int startX, int startY, int endX, int endY);

		const NodeList& GetClosedList() const { return mClosedList; }

		~DFS()
		{
			mOpenList.clear();
			mClosedList.clear();
		}

	private:
		NodeList mOpenList;
		NodeList mClosedList;
	};
}