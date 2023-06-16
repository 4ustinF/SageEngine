#include "Precompiled.h"
#include "GridBasedGraph.h"

using namespace AI;

void GridBasedGraph::Initialize(int columns, int rows)
{
	//clear the vector then set the values
	// Resize mNode to the matching dimension
	// then for each node, set its attributes (row/column)
	// however, the connection is up to the user

	//mNodes.clear();
	mNodes.resize(columns * rows);

	mColumns = columns;
	mRows = rows;

	for (int i = 0; i < rows; ++i) //20
	{
		for (int j = 0; j < columns; ++j) //30
		{
			Node node;
			node.columns = j;
			node.row = i;
			mNodes[GetIndex(j, i)] = node;
			//mNodes.push_back(node);
		}
	}
}

void GridBasedGraph::ResetSearchParams()
{
	for (auto& node : mNodes)
	{
		node.parent = nullptr;
		node.opened = false;
		node.closed = false;
		node.g = 0.0f;
		node.h = 0.0f;
	}
}

GridBasedGraph::Node* GridBasedGraph::GetNode(int x, int y)
{
	if (x < 0 || x >= mColumns ||
		y < 0 || y >= mRows) {
		return nullptr;
	}

	const int index = GetIndex(x, y);
	return &mNodes[index];
}

const GridBasedGraph::Node* GridBasedGraph::GetNode(int x, int y) const
{
	if (x < 0 || x >= mColumns ||
		y < 0 || y >= mRows) {
		return nullptr;
	}

	const int index = GetIndex(x, y);
	return &mNodes[index];
}

int GridBasedGraph::GetIndex(int x, int y) const
{
	return x + (y * GetColumns());
}