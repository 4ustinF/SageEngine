#include "TileMapService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Input;
using namespace SAGE::AI;

void TileMapService::Initialize()
{
	SetServiceName("TileMap Service");

	mInputSystem = InputSystem::Get();
	mSpriteRenderer = SpriteRenderer::Get();
	mPaintFlipMode = static_cast<int>(Pivot::TopLeft);
	mWorldOffset = { -2.0f * mTileSize, -mTileSize };
}

void TileMapService::Terminate()
{

	mClosedList.clear();

	mTiles.clear();
	mBlocked.clear();

	mColumns = 0;
	mRows = 0;

	mMap.clear();
	mSpriteRenderer = nullptr;
	mInputSystem = nullptr;
}

void TileMapService::Update(float deltaTime)
{
	if (mIsInPaintMode)
	{
		if (mInputSystem->IsMouseDown(MouseButton::LBUTTON))
		{
			PaintTile(mPaintIndex);
		}
		else if (mInputSystem->IsMouseDown(MouseButton::RBUTTON))
		{
			PaintTile(0);
		}
	}
}

void TileMapService::Render()
{
	for (int y = 0; y < mRows; ++y) {
		for (int x = 0; x < mColumns; ++x) {
			const int mapIndex = ToIndex(x, y, mColumns);
			const Tile& tile = mMap[mapIndex];
			const TextureId textureId = mTiles.at(tile.tileIndex);
			const Vector2 worldPosition = tile.localPosition + mWorldOffset;

			mSpriteRenderer->Draw(textureId, worldPosition, tile.tileRotation, tile.pivot, tile.flip);
		}
	}

	////Unblocked tiles
	//for (int r = 0; r < mRows; ++r) {
	//	for (int c = 0; c < mColumns; ++c) {
	//		const auto node = mGraph.GetNode(c, r);
	//		for (const auto neighbor : node->neighbors) {
	//			if (neighbor == nullptr) {
	//				continue;
	//			}

	//			const auto a = GetPixelPosition(node->columns, node->row);
	//			const auto b = GetPixelPosition(neighbor->columns, neighbor->row);
	//			X::DrawScreenLine(a, b, X::Colors::IndianRed);
	//		}
	//	}
	//}

	////Draw start and end spots
	//X::DrawScreenCircle(GetPixelPosition(startPosX, startPosY), tileSize * 0.25f, X::Colors::Cyan);
	//X::DrawScreenCircle(GetPixelPosition(endPosX, endPosY), tileSize * 0.25f, X::Colors::Cyan);

	//if (mClosedList.size() > 0)
	//{
	//	for (auto& node : mClosedList)
	//	{
	//		if (node->parent != nullptr)
	//		{
	//			const auto a = GetPixelPosition(node->columns, node->row);
	//			const auto b = GetPixelPosition(node->parent->columns, node->parent->row);
	//			X::DrawScreenLine(a, b, X::Colors::Cyan);
	//		}
	//	}

	//	//Iterate through the path list drawing to its parent
	//	auto node = mClosedList.back();
	//	while (node != nullptr)
	//	{
	//		X::DrawScreenCircle(GetPixelPosition(node->columns, node->row), tileSize * 0.25f, X::Colors::Cyan);
	//		node = node->parent;
	//	}
	//}
}

void TileMapService::DebugUI()
{
	ImGui::DragFloat2("World OffSet##TileMapService", &mWorldOffset.x, 0.1f);

	ImGui::Checkbox("Enable Paint Mode##TileMapService", &mIsInPaintMode);
	if (mIsInPaintMode)
	{
		// TODO: Display the tile we are painting with
		ImGui::SliderInt("Tile Index##PaintMode", &mPaintIndex, 0, static_cast<int>(mTiles.size()) - 1);
		ImGui::SliderInt("Tile Rotation##PaintMode", &mPaintPivotMode, 0, 2);
		ImGui::SliderInt("Tile Flip##PaintMode", &mPaintFlipMode, 0, static_cast<int>(Input::Flip::Count) - 1);

		if (ImGui::Button("Export Map##PaintMode"))
		{
			ExportMap();
			ExportFlipMap();
			ExportPivotMap();
		}
	}
}


void TileMapService::LoadTiles(const std::filesystem::path& fileName)
{
	//Fix load tiles 
	mTiles.clear();
	mBlocked.clear();

	FILE* file = nullptr;
	fopen_s(&file, fileName.u8string().c_str(), "r");

	int count = 0;
	fscanf_s(file, "Tiles: %d\n", &count);
	auto tm = TextureManager::Get();

	for (int i = 0; i < count; ++i) {
		char buffer[256];
		int isBlocked = 0;
		fscanf_s(file, "%s\n", buffer, static_cast<int>(std::size(buffer)));
		fscanf_s(file, "%d\n", &isBlocked);

		mTiles.push_back(tm->LoadTexture(buffer));
		isBlocked ? mBlocked.push_back(true) : mBlocked.push_back(false);
	}

	fclose(file);
}

void TileMapService::LoadTileMap(const std::filesystem::path& path)
{
	if (mMap.size() > 0) {
		mMap.clear();
	}

	mMapFilePath = path;
	FILE* file = nullptr;
	fopen_s(&file, path.u8string().c_str(), "r");

	fscanf_s(file, "Columns: %d\n", &mColumns);
	fscanf_s(file, "Rows: %d\n", &mRows);

	mMap = std::vector<Tile>(mColumns * mRows);

	for (int y = 0; y < mRows; ++y) {
		for (int x = 0; x < mColumns; ++x) {
			const int index = ToIndex(x, y, mColumns);
			int insert = fgetc(file) - '0';

			mMap[index].tileIndex = insert;
			mMap[index].localPosition = { x * mTileSize, y * mTileSize };
		}

		fgetc(file);
	}

	fclose(file);

	// --------------------------------------------------

	mGridBasedGraph.Initialize(mColumns, mRows);

	for (int r = 0; r < mRows; ++r)
	{
		for (int c = 0; c < mColumns; ++c)
		{
			if (IsBlocked(c, r)) {
				continue;
			}

			auto GetNeighbor = [&](int c, int r) -> AI::GridBasedGraph::Node*
				{
					const auto node = mGridBasedGraph.GetNode(c, r);
					if (node == nullptr) {
						return nullptr;
					}

					if (IsBlocked(c, r)) {
						return nullptr;
					}
					return node;
				};

			GridBasedGraph::Node* node = mGridBasedGraph.GetNode(c, r);
			node->neighbors[GridBasedGraph::North] = GetNeighbor(c, r - 1);
			node->neighbors[GridBasedGraph::South] = GetNeighbor(c, r + 1);
			node->neighbors[GridBasedGraph::East] = GetNeighbor(c + 1, r);
			node->neighbors[GridBasedGraph::West] = GetNeighbor(c - 1, r);
		}
	}
}

void TileMapService::LoadFlipMap(const std::filesystem::path& path)
{
	if (mMap.empty()) {
		return;
	}

	mFlipMapFilePath = path;
	FILE* file = nullptr;
	fopen_s(&file, path.u8string().c_str(), "r");

	int columns = 0;
	int rows = 0;
	fscanf_s(file, "Columns: %d\n", &columns);
	fscanf_s(file, "Rows: %d\n", &rows);
	if (columns != mColumns || rows != mRows)
	{
		return; //TODO: Throw error
	}

	for (int y = 0; y < mRows; ++y) {
		for (int x = 0; x < mColumns; ++x) {
			const int index = ToIndex(x, y, mColumns);
			int insert = fgetc(file) - '0';

			mMap[index].flip = static_cast<Flip>(insert);
		}

		fgetc(file);
	}

	fclose(file);
}

void TileMapService::LoadPivotMap(const std::filesystem::path& path)
{
	if (mMap.empty()) {
		return;
	}

	mPivotMapFilePath = path;
	FILE* file = nullptr;
	fopen_s(&file, path.u8string().c_str(), "r");

	int columns = 0;
	int rows = 0;
	fscanf_s(file, "Columns: %d\n", &columns);
	fscanf_s(file, "Rows: %d\n", &rows);
	if (columns != mColumns || rows != mRows)
	{
		return; //TODO: Throw error
	}

	for (int y = 0; y < mRows; ++y) {
		for (int x = 0; x < mColumns; ++x) {
			const int index = ToIndex(x, y, mColumns);
			int insert = fgetc(file) - '0';

			AdjustTilePivot(mMap[index], insert);
		}

		fgetc(file);
	}

	fclose(file);
}

bool TileMapService::IsBlocked(int x, int y) const
{
	const int tile = mMap[ToIndex(x, y, mColumns)].tileIndex;
	const bool blocked = mBlocked[tile];
	return blocked;
}

SAGE::Math::Vector2 TileMapService::GetPixelPosition(int x, int y) const
{
	return{
		(x + 0.5f) * mTileSize,
		(y + 0.5f) * mTileSize,
	};
}

SAGE::Math::Rect TileMapService::GetBound() const
{
	// TODO: Take offset into consideration
	return {
		0.0f,					//Left
		0.0f,					//Top
		mColumns * mTileSize,	//right
		mRows * mTileSize		//Bottom
	};
}

std::vector<SAGE::Math::Vector2> TileMapService::FindPath(int startPosX, int startPosY, int endPosX, int endPosY)
{
	std::vector<SAGE::Math::Vector2> path;

	auto getCostWrapper = [&](auto nodeA, auto nodeB)
		{
			return GetCost(nodeA, nodeB);
		};

	auto getHeuristicWrapper = [&](auto nodeA, auto nodeB)
		{
			return GetHeuristic(nodeA, nodeB);
		};

	aStar star;
	if (star.Run(mGridBasedGraph, startPosX, startPosY, endPosX, endPosY, getCostWrapper, getHeuristicWrapper))
	{
		const auto& closedList = star.GetClosedList();
		auto node = closedList.back();
		while (node != nullptr)
		{
			path.push_back(GetPixelPosition(node->columns, node->row));
			node = node->parent;
		}
		std::reverse(path.begin(), path.end());

		// Cache the closed list for visualization
		mClosedList = closedList;
	}

	return path;
}

float TileMapService::GetCost(const GridBasedGraph::Node* nodeA, const GridBasedGraph::Node* nodeB) const
{
	const int fromTileIndex = ToIndex(nodeA->columns, nodeA->row, mColumns);
	const int toTileIndex = ToIndex(nodeB->columns, nodeB->row, mColumns);

	const int tileType = mMap[toTileIndex].tileIndex;
	const float multiplier = ((nodeA->columns != nodeB->columns) && (nodeA->row != nodeB->row)) ? 1.414f : 1.0f;

	multiplier * 1.01f; //Multiplier a tie breaker to eliminate symmetry

	//// Makes it harder to go through that tile example
	//if (tileType == 1) {
	//	return 5.0f * multiplier;
	//}

	return 1.0f * multiplier;
}

float TileMapService::GetHeuristic(const GridBasedGraph::Node* nodeA, const GridBasedGraph::Node* nodeB) const
{
	//sqrt(A^2 + b^2) = c
	return sqrt((nodeA->columns - nodeB->columns) * (nodeA->columns - nodeB->columns) +
		(nodeA->row - nodeB->row) * (nodeA->row - nodeB->row));
}

void TileMapService::PaintTile(int tileIndex)
{
	if (tileIndex >= mTiles.size())
	{
		return;
	}

	const int tilePosX = static_cast<int>((mInputSystem->GetMouseScreenX() - mWorldOffset.x) / mTileSize);
	const int tilePosY = static_cast<int>((mInputSystem->GetMouseScreenY() - mWorldOffset.y) / mTileSize);
	Tile& tile = mMap[ToIndex(tilePosX, tilePosY, mColumns)];
	tile.tileIndex = tileIndex;
	tile.flip = static_cast<Flip>(mPaintFlipMode);

	AdjustTilePivot(tile, mPaintPivotMode);
}

void TileMapService::ExportMap()
{
	if (mMapFilePath.empty())
	{
		mFlipMapFilePath = "map.txt";
	}

	FILE* file = nullptr;
	fopen_s(&file, mMapFilePath.u8string().c_str(), "w");

	fprintf(file, "Columns: %d\n", mColumns);
	fprintf(file, "Rows: %d\n", mRows);

	for (int y = 0; y < mRows; ++y) {
		for (int x = 0; x < mColumns; ++x) {
			const int index = ToIndex(x, y, mColumns);
			fprintf(file, "%d", mMap[index].tileIndex);
		}

		fprintf(file, "\n");
	}

	fclose(file);
}

void TileMapService::ExportFlipMap()
{
	if (mFlipMapFilePath.empty())
	{
		mFlipMapFilePath = "flipmap.txt";
	}

	FILE* file = nullptr;
	fopen_s(&file, mFlipMapFilePath.u8string().c_str(), "w");

	fprintf(file, "Columns: %d\n", mColumns);
	fprintf(file, "Rows: %d\n", mRows);

	for (int y = 0; y < mRows; ++y) {
		for (int x = 0; x < mColumns; ++x) {
			const int index = ToIndex(x, y, mColumns);
			fprintf(file, "%d", static_cast<int>(mMap[index].flip));
		}

		fprintf(file, "\n");
	}

	fclose(file);
}

void TileMapService::ExportPivotMap()
{
	if (mPivotMapFilePath.empty())
	{
		mPivotMapFilePath = "pivotmap.txt";
	}

	FILE* file = nullptr;
	fopen_s(&file, mPivotMapFilePath.u8string().c_str(), "w");

	fprintf(file, "Columns: %d\n", mColumns);
	fprintf(file, "Rows: %d\n", mRows);

	for (int y = 0; y < mRows; ++y) {
		for (int x = 0; x < mColumns; ++x) {
			const int index = ToIndex(x, y, mColumns);
			fprintf(file, "%d", PivotToIndex(mMap[index].pivot));
		}

		fprintf(file, "\n");
	}

	fclose(file);
}

void TileMapService::AdjustTilePivot(Tile& tile, int pivotIndex)
{
	switch (pivotIndex)
	{
	case 0:
		tile.tileRotation = 0.0f;
		tile.pivot = Pivot::TopLeft;
		break;
	case 1:
		tile.tileRotation = -Constants::HalfPi;
		tile.pivot = Pivot::TopRight;
		break;
	case 2:
		tile.tileRotation = Constants::HalfPi;
		tile.pivot = Pivot::BottomLeft;
		break;
	}
}

int TileMapService::PivotToIndex(SAGE::Input::Pivot pivot) const
{
	switch (pivot)
	{
	case Pivot::TopLeft: return 0;
	case Pivot::TopRight: return 1;
	case Pivot::BottomLeft: return 2;
	case Pivot::BottomRight: return 3;
	default:
		return 0;
	}
}
