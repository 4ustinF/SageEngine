#pragma once

namespace AI
{
	class GridBasedGraph
	{
	public:
		enum Direction
		{
			North, //0
			South, 
			East,
			West,
			NorthEast,
			NorthWest,
			SouthEast,
			SouthWest //7
		};

		struct Node
		{
			//Graph structure - set in intialize, does not change during search
			std::array<Node*, 8> neighbors = {};
			int columns = 0;
			int row = 0;

			//Search parameters, shouuld not be reset before each search 
			Node* parent = nullptr;
			bool opened = false;
			bool closed = false;
			float g = 0.0f;
			float h = 0.0f;
		};

		void Initialize(int columns, int rows);
		void ResetSearchParams();
		Node* GetNode(int x, int y);
		const Node* GetNode(int x, int y) const;

		int GetColumns() const { return mColumns; }
		int GetRows() const { return mRows; }

	private:
		int GetIndex(int x, int y) const;

		std::vector<Node> mNodes;
		int mColumns = 0;
		int mRows = 0;
	};
}