#include "TileMapService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Input;

void TileMapService::Initialize()
{
	SetServiceName("TileMap Service");

	mInputSystem = InputSystem::Get();
	mPaintFlipMode = static_cast<int>(Pivot::TopLeft);
	mWorldOffset = { -mTileSize, -mTileSize };
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
		(y + 0.5f)* mTileSize,
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
