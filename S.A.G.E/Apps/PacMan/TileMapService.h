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

	void SetTileSize(float tileSize) { mTileSize = tileSize; };
	float GetTileSize() const { return mTileSize; };

	bool IsBlocked(int x, int y) const;
	SAGE::Math::Rect GetBound() const;

private:
	SAGE::Math::Vector2 GetPixelPosition(int x, int y) const;
	void PaintTile(int tileIndex);
	void ExportMap();
	void ExportFlipMap();
	void ExportPivotMap();

	void AdjustTilePivot(Tile& tile, int pivotIndex);
	int PivotToIndex(SAGE::Input::Pivot pivot) const;

	// References
	SAGE::Input::InputSystem* mInputSystem;
	std::filesystem::path mMapFilePath;
	std::filesystem::path mFlipMapFilePath;
	std::filesystem::path mPivotMapFilePath;

	std::vector<Tile> mMap;
	std::vector<bool> mBlocked;
	std::vector<SAGE::Graphics::TextureId> mTiles;
	int mColumns = 0;
	int mRows = 0;

	float mTileSize = 24.0f;
	float mWorldOffset = 0.0f;

	// Painting
	bool mIsInPaintMode = false;
	int mPaintIndex = 0;
	int mPaintFlipMode = 0;
	int mPaintPivotMode = 0;
};