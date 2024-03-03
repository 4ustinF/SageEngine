#include "TileMapService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Input;

namespace
{
	//std::unique_ptr<TileMap> tileMapInstance;

	inline int ToIndex(int x, int y, int columns) {
		return x + (y * columns);
	}
}

void TileMapService::Initialize()
{
	SetServiceName("TileMap Service");
}

void TileMapService::Terminate()
{
	mTiles.clear();
	mBlocked.clear();

	mColumns = 0;
	mRows = 0;

	mMap.clear();
}

void TileMapService::Update(float deltaTime)
{
}

void TileMapService::Render()
{
	//Map
	for (int y = 0; y < mRows; ++y) {
		for (int x = 0; x < mColumns; ++x) {
			const int mapIndex = ToIndex(x, y, mColumns);
			const int tileIndex = mMap[mapIndex];
			const TextureId textureId = mTiles.at(tileIndex);
			const Vector2 worldPosition = { x * mTileSize, y * mTileSize };

			SpriteRenderer::Get()->Draw(textureId, worldPosition, 0.0f, Pivot::TopLeft, Flip::None);
		}
	}
}

void TileMapService::DebugUI()
{

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

void TileMapService::LoadMap(const char* fileName)
{
	if (mMap.size() > 0) {
		mMap.clear();
	}

	FILE* file = nullptr;
	std::filesystem::path path = fileName;
	fopen_s(&file, path.u8string().c_str(), "r");

	fscanf_s(file, "Columns: %d\n", &mColumns);
	fscanf_s(file, "Rows: %d\n", &mRows);

	mMap = std::vector<int>(mColumns * mRows);

	for (int y = 0; y < mRows; ++y) {
		for (int x = 0; x < mColumns; ++x) {
			const int index = ToIndex(x, y, mColumns);
			int insert = fgetc(file) - '0';
			mMap[index] = insert;
		}

		fgetc(file);
	}

	fclose(file);
}

bool TileMapService::IsBlocked(int x, int y) const
{
	const int tile = mMap[ToIndex(x, y, mColumns)];
	const bool blocked = mBlocked[tile];
	return blocked;
}

SAGE::Math::Vector2 TileMapService::GetPixelPosition(int x, int y) const
{
	return{
		(x + 0.5f) * mTileSize,
		(y + 0.5f)* mTileSize,
	};
}

SAGE::Math::Rect TileMapService::GetBound() const
{
	return {
		0.0f,					//Left
		0.0f,					//Top
		mColumns * mTileSize,	//right
		mRows * mTileSize		//Bottom
	};
}
