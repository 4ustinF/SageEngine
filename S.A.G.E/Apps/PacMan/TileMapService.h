#pragma once

#include "TypeIds.h"

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
	void LoadMap(const char* fileName);

	bool IsBlocked(int x, int y) const;
	SAGE::Math::Rect GetBound() const;

private:
	SAGE::Math::Vector2 GetPixelPosition(int x, int y) const;

	std::vector<int> mMap;
	std::vector<bool> mBlocked;
	std::vector<SAGE::Graphics::TextureId> mTiles;
	int mColumns = 0;
	int mRows = 0;

	const float mTileSize = 24.0f;
};