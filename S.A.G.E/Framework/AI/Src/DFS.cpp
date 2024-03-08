#include "Precompiled.h"
#include "DFS.h"

using namespace SAGE::AI;

bool DFS::Run(GridBasedGraph& graph, int startX, int startY, int endX, int endY)
{
	// Reset everything
	graph.ResetSearchParams();
	mOpenList.clear();
	mClosedList.clear();

	// Add start node to open list
	auto node = graph.GetNode(startX, startY);
	mOpenList.push_back(node);
	node->opened = true;

	node->parent = nullptr;

	bool found = false;

	//Open list - list of nodes you need to considor in the steps ahead
	//Closed list - list of nodes you already visited and dont need to consider again
	//Parent - for each node, keep track of which node it was expanded from
	//LIFO

	while (!found && !mOpenList.empty())
	{
		auto node = mOpenList.back();
		mOpenList.pop_back();

		if (node->columns == endX && node->row == endY) {
			found = true;
		}
		else {
			for (auto neighbor : node->neighbors)
			{
				//if expanded node isnt open or closed list and isnt null
				if (neighbor == nullptr || neighbor->opened || neighbor->closed) {
					continue;
				}

				//Add expanded node to the open list, set parent node to N
				neighbor->opened = true;
				neighbor->parent = node;
				mOpenList.push_back(neighbor);
			}
		}

		node->closed = true;
		mClosedList.push_back(node);
	}
	return found;
}

