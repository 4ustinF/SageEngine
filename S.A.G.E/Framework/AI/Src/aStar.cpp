#include "Precompiled.h"
#include "aStar.h"

using namespace AI;

bool aStar::Run(GridBasedGraph& graph, int startX, int startY, int endX, int endY, GetCost getCost, GetHeuristics getHeur)
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
		//auto node = mOpenList.back();
		//mOpenList.pop_back();

		auto node = mOpenList.front();
		mOpenList.pop_front();

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
					neighbor->h = getHeur(neighbor, graph.GetNode(endX, endY));
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

void aStar::SortInsertOpenList(AI::GridBasedGraph::Node* node)
{
	if (mOpenList.empty())
	{
		mOpenList.push_back(node);
		return;
	}

	for (std::list<GridBasedGraph::Node*>::iterator it = mOpenList.begin(); it != mOpenList.end(); ++it)
	{
		if ((node->g + node->h) <= ((*it)->g + (*it)->h))
		{
			mOpenList.insert(it, node);
			return;
		}
	}
	mOpenList.push_back(node);
}
