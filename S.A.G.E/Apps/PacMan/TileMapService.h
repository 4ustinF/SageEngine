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
	void LoadMap(const std::filesystem::path& fileName);

	bool IsBlocked(int x, int y) const;
	SAGE::Math::Rect GetBound() const;

private:
	SAGE::Math::Vector2 GetPixelPosition(int x, int y) const;
	void PaintTile(int tileIndex);
	void ExportMap();

	// References
	SAGE::Input::InputSystem* mInputSystem;
	std::filesystem::path mMapFilePath;

	std::vector<Tile> mMap;
	std::vector<bool> mBlocked;
	std::vector<SAGE::Graphics::TextureId> mTiles;
	int mColumns = 0;
	int mRows = 0;

	const float mTileSize = 24.0f;
	float mWorldOffset = 0.0f;

	bool mIsInPaintMode = false;
	int mPaintIndex = 0;
	float mPaintRotation = 0.0f;
	int mPaintFlipMode = 0;
	int mPaintPivotMode = 0;
};