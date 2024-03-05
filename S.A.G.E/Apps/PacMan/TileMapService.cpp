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

	mInputSystem = InputSystem::Get();
	mPaintFlipMode = static_cast<int>(Pivot::TopLeft);
	mPaintPivotMode = static_cast<int>(Flip::None);
}

void TileMapService::Terminate()
{
	mTiles.clear();
	mBlocked.clear();

	mColumns = 0;
	mRows = 0;

	mMap.clear();
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

			SpriteRenderer::Get()->Draw(textureId, worldPosition, tile.tileRotation, tile.pivot, tile.flip);
		}
	}
}

void TileMapService::DebugUI()
{
	ImGui::Checkbox("Enable Paint Mode", &mIsInPaintMode);
	if (mIsInPaintMode)
	{
		// TODO: Display the tile we are painting with
		ImGui::SliderInt("Tile Index", &mPaintIndex, 0, static_cast<int>(mTiles.size()) - 1);
		ImGui::SliderAngle("Tile Rotation", &mPaintRotation);
		ImGui::SliderInt("Tile Flip", &mPaintFlipMode, 0, static_cast<int>(Input::Flip::Count) - 1);
		ImGui::SliderInt("Tile Flip", &mPaintFlipMode, 0, static_cast<int>(Input::Pivot::Count) - 1);

		if (ImGui::Button("Export Map"))
		{
			ExportMap();
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

void TileMapService::LoadMap(const std::filesystem::path& path)
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

void TileMapService::PaintTile(int tileIndex)
{
	if (tileIndex >= mTiles.size())
	{
		return;
	}

	const int tilePosX = static_cast<int>(mInputSystem->GetMouseScreenX() / mTileSize); // TODO: Take offset into consideration
	const int tilePosY = static_cast<int>(mInputSystem->GetMouseScreenY() / mTileSize); // TODO: Take offset into consideration
	Tile& tile = mMap[ToIndex(tilePosX, tilePosY, mColumns)];
	tile.tileIndex = tileIndex;
	tile.tileRotation = mPaintRotation;
	tile.flip = static_cast<Flip>(mPaintFlipMode);
	tile.pivot = static_cast<Pivot>(mPaintPivotMode);
}

void TileMapService::ExportMap()
{
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
