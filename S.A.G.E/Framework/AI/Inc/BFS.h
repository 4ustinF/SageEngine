#pragma once

#include "GridBasedGraph.h"

namespace SAGE::AI
{
	using NodeList = std::list<GridBasedGraph::Node*>;

	class BFS
	{
	public:
		bool Run(GridBasedGraph& graph, int startX, int startY, int endX, int endY);

		const NodeList& GetClosedList() const { return mClosedList; }

		~BFS()
		{
			mOpenList.clear();
			mClosedList.clear();
		}

	private:
		NodeList mOpenList;
		NodeList mClosedList;
	};
}