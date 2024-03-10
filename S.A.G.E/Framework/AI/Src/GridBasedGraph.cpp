#include "Precompiled.h"
#include "GridBasedGraph.h"

using namespace SAGE::AI;

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

void GridBasedGraph::BlockTile(int x, int y)
{
	if (x < 0 || x >= mColumns ||
		y < 0 || y >= mRows) {
		return;
	}

	Node* node = GetNode(x, y);

	Node* northNode = node->neighbors[static_cast<int>(GridBasedGraph::North)];
	if (northNode != nullptr) {
		northNode->neighbors[static_cast<int>(GridBasedGraph::South)] = nullptr;
		northNode = nullptr;
	}

	Node* southNode = node->neighbors[static_cast<int>(GridBasedGraph::South)];
	if (southNode != nullptr) {
		southNode->neighbors[static_cast<int>(GridBasedGraph::North)] = nullptr;
		southNode = nullptr;
	}

	Node* eastNode = node->neighbors[static_cast<int>(GridBasedGraph::East)];
	if (eastNode != nullptr) {
		eastNode->neighbors[static_cast<int>(GridBasedGraph::West)] = nullptr;
		eastNode = nullptr;
	}

	Node* westNode = node->neighbors[static_cast<int>(GridBasedGraph::West)];	
	if (westNode != nullptr) {
		westNode->neighbors[static_cast<int>(GridBasedGraph::East)] = nullptr;
		westNode = nullptr;
	}

	Node* northEastNode = node->neighbors[static_cast<int>(GridBasedGraph::NorthEast)];
	if (northEastNode != nullptr) {
		northEastNode->neighbors[static_cast<int>(GridBasedGraph::SouthWest)] = nullptr;
		northEastNode = nullptr;
	}

	Node* northWestNode = node->neighbors[static_cast<int>(GridBasedGraph::NorthWest)];	
	if (northWestNode != nullptr) {
		northWestNode->neighbors[static_cast<int>(GridBasedGraph::SouthEast)] = nullptr;
		northWestNode = nullptr;
	}

	Node* southEastNode = node->neighbors[static_cast<int>(GridBasedGraph::SouthEast)];
	if (southEastNode != nullptr) {
		southEastNode->neighbors[static_cast<int>(GridBasedGraph::NorthWest)] = nullptr;
		southEastNode = nullptr;
	}

	Node* southWestNode = node->neighbors[static_cast<int>(GridBasedGraph::SouthWest)];
	if (southWestNode != nullptr) {
		southWestNode->neighbors[static_cast<int>(GridBasedGraph::NorthEast)] = nullptr;
		southWestNode = nullptr;
	}
}


int GridBasedGraph::GetIndex(int x, int y) const
{
	return x + (y * GetColumns());
}