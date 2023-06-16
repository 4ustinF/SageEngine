#include "Precompiled.h"
#include "Dijkstra.h"

using namespace AI;

bool Dijkstra::Run(GridBasedGraph& graph, int startX, int startY, int endX, int endY, GetCost getCost)
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
				//if expanded node isnt closed list and isnt null
				if (neighbor == nullptr || neighbor->closed) {
					continue;
				}

				if (!neighbor->opened) { //Add expanded node to the open list, set parent node to N
				neighbor->opened = true;
				neighbor->parent = node;
				neighbor->g = node->g + getCost(node, neighbor);
				SortInsertOpenList(neighbor);
				}
				else //neighbor is open - see if this g value is better if so then change its parent and g value
				{
					//Compare whos g value is lower
					float neighborsGVal = node->g + getCost(node, neighbor);
					if (neighborsGVal < neighbor->g) //Edge relaxtion
					{
						neighbor->g = neighborsGVal;
						neighbor->parent = node;

						for (std::list<GridBasedGraph::Node*>::iterator it = mOpenList.begin(); it != mOpenList.end(); ++it)
						{
							if (*(it) == neighbor)
							{
								mOpenList.remove(*(it));
								break;
							}
						}

						SortInsertOpenList(neighbor);
					}
				}				
			}
		}

		node->closed = true;
		mClosedList.push_back(node);
	}
	return found;
}

void Dijkstra::SortInsertOpenList(AI::GridBasedGraph::Node* node)
{
	if (mOpenList.empty())
	{
		mOpenList.push_back(node);
		return;
	}

	for (std::list<GridBasedGraph::Node*>::iterator it = mOpenList.begin(); it != mOpenList.end(); ++it)
	{
		if (node->g >= (*it)->g)
		{
			mOpenList.insert(it, node);
			break;
		}
	}
}
