#pragma once

#include "TypeIds.h"

struct Tile
{
	int tileIndex = 0;
	float tileRotation = 0.0f;
	SAGE::Math::Vector2 localPosition = SAGE::Math::Vector2::Zero;
	SAGE::Input::Flip flip = SAGE::Input::Flip::None;
	SAGE::Input::Pivot pivot = SAGE::Input::Pivot::TopLeft;
};

class TileMapService final : public SAGE::Service
{
public:
	SET_TYPE_ID(ServiceId::TileMap);

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

	void LoadTiles(const std::filesystem::path& fileName);
	void LoadTileMap(const std::filesystem::path& fileName);
	void LoadFlipMap(const std::filesystem::path& fileName);
	void LoadPivotMap(const std::filesystem::path& fileName);

	// TODO: Move to functions to do safety checks
	int ToIndex(int x, int y, int columns) const { return x + (y * columns); }
	Tile& GetTile(SAGE::Math::Vector2Int pos) { return mMap[ToIndex(pos.x, pos.y, mColumns)]; }
	Tile& GetTile(int x, int y) { return mMap[ToIndex(x, y, mColumns)]; }
	void SetTile(int x, int y, const Tile& tile) { mMap[ToIndex(x, y, mColumns)] = tile; }
	void SetTile(SAGE::Math::Vector2Int pos, const Tile& tile) { mMap[ToIndex(pos.x, pos.y, mColumns)] = tile; }

	SAGE::Math::Vector2 GetWorldOffset() const { return mWorldOffset; }
	void SetTileSize(float tileSize) { mTileSize = tileSize; };
	float GetTileSize() const { return mTileSize; };
	int GetColumns() const { return mColumns; }
	int GetRows() const { return mRows; }

	bool IsBlocked(SAGE::Math::Vector2Int cords) const;
	bool IsBlocked(int x, int y) const;
	SAGE::Math::Rect GetBound() const;

	// Path finding
	std::vector<SAGE::Math::Vector2> FindPath(int startPosX, int startPosY, int endPosX, int endPosY);

	SAGE::AI::GridBasedGraph mGridBasedGraph;

private:
	SAGE::Math::Vector2 GetPixelPosition(int x, int y) const;
	void PaintTile(int tileIndex);
	void ExportMap();
	void ExportFlipMap();
	void ExportPivotMap();

	void AdjustTilePivot(Tile& tile, int pivotIndex);
	int PivotToIndex(SAGE::Input::Pivot pivot) const;

	// References
	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::Graphics::SpriteRenderer* mSpriteRenderer = nullptr;
	std::filesystem::path mMapFilePath;
	std::filesystem::path mFlipMapFilePath;
	std::filesystem::path mPivotMapFilePath;

	std::vector<Tile> mMap;
	std::vector<bool> mBlocked;
	std::vector<SAGE::Graphics::TextureId> mTiles;
	int mColumns = 0;
	int mRows = 0;

	float mTileSize = 24.0f;
	SAGE::Math::Vector2 mWorldOffset = SAGE::Math::Vector2::Zero;

	// Painting
	bool mIsInPaintMode = false;
	int mPaintIndex = 0;
	int mPaintFlipMode = 0;
	int mPaintPivotMode = 0;

	// Path finding
	float GetCost(const SAGE::AI::GridBasedGraph::Node* nodeA, const SAGE::AI::GridBasedGraph::Node* nodeB) const;
	float GetHeuristic(const SAGE::AI::GridBasedGraph::Node* nodeA, const SAGE::AI::GridBasedGraph::Node* nodeB) const;
	SAGE::AI::NodeList mClosedList;

};