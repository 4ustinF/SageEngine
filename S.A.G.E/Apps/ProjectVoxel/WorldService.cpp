#include "WorldService.h"
#include "PlayerUIService.h"
#include "Enums.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Coroutine;

const std::string WorldService::KeyGenerator(const Vector2Int& coords)
{
	return std::to_string(coords.x) + ", " + std::to_string(coords.y);
}

void WorldService::Initialize()
{
	SetServiceName("World Service");
	mCameraService = GetWorld().GetService<CameraService>();

	mVoxelTerrainEffect.SetBlendState(BlendState::Mode::AlphaBlend);
	mVoxelTerrainEffect.Initialize(Sampler::Filter::Point);
	mVoxelTerrainEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mVoxelTerrainEffect.SetDirectionalLight(GetWorld().GetService<RenderService>()->GetDirectionalLight());
	mVoxelTerrainEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mVoxelTerrainEffect.SetDepthBias(0.000040f);
	mVoxelTerrainEffect.SetFogColor(mFogColor);
	mVoxelTerrainEffect.UseFog(mUseFog);

	mVoxelWaterEffect.SetBlendState(BlendState::Mode::AlphaBlend);
	mVoxelWaterEffect.Initialize(Sampler::Filter::Point);
	mVoxelWaterEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mVoxelWaterEffect.SetDirectionalLight(GetWorld().GetService<RenderService>()->GetDirectionalLight());
	mVoxelWaterEffect.SetChunkSize(static_cast<float>(mChunkSize.x));
	mVoxelWaterEffect.SetFogColor(mFogColor);
	mVoxelWaterEffect.UseFog(mUseFog);
	UpdateFogAmount();

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(GetWorld().GetService<RenderService>()->GetDirectionalLight());
	mShadowEffect.SetSize(200.0f);

	// Set Noise
	InitializeNoise();

	mRenderDistance = Clamp(mRenderDistance, 1, 15);
	mNewRenderDistance = mRenderDistance;

	// Create Starting chunks
	for (int x = -mRenderDistance; x <= mRenderDistance; ++x) {
		for (int z = -mRenderDistance; z <= mRenderDistance; ++z) {
			CreateChunkRenderObject(KeyGenerator({ x, z }));
		}
	}

	//auto start = std::chrono::high_resolution_clock::now();
	// Refresh starting chunks
	RefreshChunks();
	//auto stop = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

	reloadChunks = std::thread([this] { this->ChunkLoadCheck(); });
}

void WorldService::Terminate()
{
	RemoveOldChunkRigidBodies();

	mAABBs.clear();

	mWorkerThreadActive = false;
	//while (!reloadChunks.joinable())
	//{
	//	Sleep(static_cast<DWORD>(0.01f));
	//}
	reloadChunks.join();

	mChunkModifications.clear();
	std::queue<ChunkToReload> empty;
	std::swap(mChunksToReload, empty);

	mShadowEffect.Terminate();
	mVoxelWaterEffect.Terminate();
	mVoxelTerrainEffect.Terminate();
	mCameraService = nullptr;

	mChunkData.clear();
	for (auto& chunk : mChunkRenderObjects)
	{
		chunk.second.RemoveChunkCollider();
	}
	mChunkRenderObjects.clear();
}

void WorldService::Update(float deltaTime)
{
	mVoxelWaterEffect.SetDeltaTime(deltaTime);

	// Moved Chunks
	if (mOldWorldCoords != mNewWorldCoords)
	{
		if (mNewWorldCoords.x > mOldWorldCoords.x) { // Moving Right 
			AssignReloadWall(ChunkWall::West);
		}
		else if (mNewWorldCoords.x < mOldWorldCoords.x) { // Moving Left
			AssignReloadWall(ChunkWall::East);
		}
		else if (mNewWorldCoords.y > mOldWorldCoords.y) { // Moving Up
			AssignReloadWall(ChunkWall::South);
		}
		else { // Moving Down
			AssignReloadWall(ChunkWall::North);
		}

		const auto& playerUIService = GetWorld().GetService<PlayerUIService>();
		playerUIService->SetChunkCoord(mNewWorldCoords);
		playerUIService->SetCurrentBiome(mChunkData.at(KeyGenerator(mNewWorldCoords)).biomeType);

		mOldWorldCoords = mNewWorldCoords;
		mShadowEffect.SetFocus(mCameraService->GetCamera().GetPosition());
	}

	Tick(deltaTime);

	RemoveOldChunkRigidBodies();
}

void WorldService::Render()
{
	mVoxelTerrainEffect.SetCamera(mCameraService->GetCamera());
	mVoxelWaterEffect.SetCamera(mCameraService->GetCamera());

	std::vector<std::string> chunkKeys;
	mVoxelTerrainEffect.Begin();
	{
		for (int x = mNewWorldCoords.x - mRenderDistance; x <= mNewWorldCoords.x + mRenderDistance; ++x) {
			for (int z = mNewWorldCoords.y - mRenderDistance; z <= mNewWorldCoords.y + mRenderDistance; ++z) {
				const std::string key = KeyGenerator({ x, z });
				if (mChunkRenderObjects.find(key) != mChunkRenderObjects.end()) {
					const auto& chunk = mChunkRenderObjects.at(key);
					mVoxelTerrainEffect.Render(chunk.opaqueChunkRenderObject);
					chunkKeys.push_back(key);
				}
			}
		}
	}
	mVoxelTerrainEffect.End();

	mVoxelWaterEffect.Begin();
	{
		for (auto& key : chunkKeys) {
			if (mChunkRenderObjects.find(key) != mChunkRenderObjects.end() && mChunkRenderObjects.at(key).waterChunkActive)
			{
				mVoxelWaterEffect.Render(mChunkRenderObjects.at(key).waterChunkRenderObject);
			}
		}
	}
	mVoxelWaterEffect.End();

	mVoxelTerrainEffect.Begin();
	{
		for (auto& key : chunkKeys) {
			if (mChunkRenderObjects.find(key) != mChunkRenderObjects.end() && mChunkRenderObjects.at(key).transChunkActive)
			{
				mVoxelTerrainEffect.Render(mChunkRenderObjects.at(key).transparentChunkRenderObject);
			}
		}
	}
	mVoxelTerrainEffect.End();

	mShadowEffect.Begin();
	{
		for (auto& key : chunkKeys) {
			if (mChunkRenderObjects.find(key) != mChunkRenderObjects.end())
			{
				mShadowEffect.Render(mChunkRenderObjects.at(key).opaqueChunkRenderObject);
				if (mChunkRenderObjects.at(key).transChunkActive)
				{
					mShadowEffect.Render(mChunkRenderObjects.at(key).transparentChunkRenderObject);
				}
			}
		}
	}
	mShadowEffect.End();
}

void WorldService::DebugUI()
{
	if (ImGui::Checkbox("Use Fog##VoxelWorldService", &mUseFog))
	{
		if (mRenderDistance <= 3)
		{
			mUseFog = !mUseFog;
		}
		else
		{
			mVoxelTerrainEffect.UseFog(mUseFog);
			mVoxelWaterEffect.UseFog(mUseFog);
		}
	}

	ImGui::Checkbox("Lift Chunks##VoxelWorldService", &mLiftChunk);

	ImGui::Separator();
	mVoxelTerrainEffect.DebugUI();
	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();
	mVoxelWaterEffect.DebugUI();
	ImGui::Separator();
	if (ImGui::SliderInt("Render Distance", &mNewRenderDistance, 1, 15)) {
		UpdateRenderDistance();
	}
}


void WorldService::ChunkLoadCheck()
{
	while (mWorkerThreadActive)
	{
		// Continuous land
		while (!mChunksToReload.empty())
		{
			ReloadChunk(mChunksToReload.front());
			mChunksToReload.pop();
		}
	}
}

void WorldService::InitializeNoise()
{
	// Terrain 
	mNoise.SetNoiseType(Noise::NoiseType_Perlin);
	mNoise.SetFractalType(Noise::FractalType_FBm);
	mNoise.SetFrequency(0.01f);
	mNoise.SetFractalLacunarity(2.0f);
	mNoise.SetFractalGain(0.5f);

	// Surface Noise
	mSurfaceNoise.SetNoiseType(Noise::NoiseType_Perlin);
	mSurfaceNoise.SetFractalType(Noise::FractalType_FBm);
	mSurfaceNoise.SetFrequency(0.2f);
	mSurfaceNoise.SetFractalLacunarity(1.9f);
	mSurfaceNoise.SetFractalGain(0.6f);

	// Coal
	mCoalNoise.SetNoiseType(Noise::NoiseType_OpenSimplex2);
	mCoalNoise.SetFractalType(Noise::FractalType_DomainWarpProgressive);
	mCoalNoise.SetFrequency(0.05f);
	mCoalNoise.SetFractalLacunarity(2.2f);
	mCoalNoise.SetFractalGain(0.2f);

	// Tree Zone
	mTreeZoneNoise.SetNoiseType(Noise::NoiseType_OpenSimplex2);
	mTreeZoneNoise.SetFractalType(Noise::FractalType_DomainWarpProgressive);
	mTreeZoneNoise.SetFrequency(0.02f);
	mTreeZoneNoise.SetFractalLacunarity(2.2f);
	mTreeZoneNoise.SetFractalGain(0.7f);

	// Tree
	mTreeNoise.SetNoiseType(Noise::NoiseType_Perlin);
	mTreeNoise.SetFractalType(Noise::FractalType_None);
	mTreeNoise.SetFrequency(0.05f);

	// Humidity
	mHumidityNoise.SetNoiseType(Noise::NoiseType_OpenSimplex2);
	mHumidityNoise.SetFractalType(Noise::FractalType_None);
	//mHumidityNoise.SetFrequency(0.06f);
	mHumidityNoise.SetFrequency(0.02f);
	mHumidityNoise.SetFractalLacunarity(0.9f);
	mHumidityNoise.SetFractalGain(0.3f);
	mHumidityNoise.SetSeed(5278678);

	// Temperature
	mTemperatureNoise.SetNoiseType(Noise::NoiseType_OpenSimplex2S);
	mTemperatureNoise.SetFractalType(Noise::FractalType_None);
	//mTemperatureNoise.SetFrequency(0.04f);
	mTemperatureNoise.SetFrequency(0.01f);
	mTemperatureNoise.SetFractalLacunarity(0.6f);
	mTemperatureNoise.SetFractalGain(0.6f);
	mTemperatureNoise.SetSeed(764234);

	mBiomeBlendingNoise.SetNoiseType(Noise::NoiseType_OpenSimplex2);
	mBiomeBlendingNoise.SetFractalType(Noise::FractalType_DomainWarpProgressive);
	mBiomeBlendingNoise.SetFractalOctaves(3);
	mBiomeBlendingNoise.SetFrequency(0.5f);
	mBiomeBlendingNoise.SetFractalLacunarity(2.2f);
	mBiomeBlendingNoise.SetFractalGain(0.7f);
}

BiomeTypes WorldService::GetBiomeType(Vector2Int chunkCoords)
{
	float biomeHumidityNoise = mHumidityNoise.GetNoise(static_cast<float>(chunkCoords.x + 5), static_cast<float>(chunkCoords.y + 20));
	float biomeTemperatureNoise = mTemperatureNoise.GetNoise(static_cast<float>(chunkCoords.x + 5), static_cast<float>(chunkCoords.y + 20));

	if (biomeHumidityNoise < 0.0f)
	{
		if (biomeTemperatureNoise < -0.34f)
		{
			return BiomeTypes::Plains;
		}
		else if (biomeTemperatureNoise < 0.34f)
		{
			return BiomeTypes::Mountains;
		}
		return BiomeTypes::Desert;
	}

	if (biomeTemperatureNoise < -0.34f)
	{
		return BiomeTypes::SnowyTundra;
	}
	else if (biomeTemperatureNoise < 0.34f)
	{
		return BiomeTypes::Ocean;
	}

	return BiomeTypes::Forest;
}

void WorldService::InitializeMesh(Vector2Int chunkCoords, ChunkData& chunkData)
{
	BlockInfo data[18][32][18];

	// TODO: Turn into a 1d vector
	// Set inside
	for (int x = 0; x < 16; ++x) {
		for (int z = 0; z < 16; ++z) {
			for (int y = 0; y < 32; ++y) {
				data[x + 1][y][z + 1] = chunkData.blocksData[x][y][z];
			}
		}
	}

	// Set Front
	std::string key = KeyGenerator({ chunkCoords.x, chunkCoords.y + 1 });
	if (mChunkData.find(key) == mChunkData.end()) {
		mChunkData.insert(std::make_pair(key, CreateChunkData({ chunkCoords.x, chunkCoords.y + 1 })));
	}
	auto& chunkFront = mChunkData.at(key);

	// Set Back
	key = KeyGenerator({ chunkCoords.x, chunkCoords.y - 1 });
	if (mChunkData.find(key) == mChunkData.end()) {
		mChunkData.insert(std::make_pair(key, CreateChunkData({ chunkCoords.x, chunkCoords.y - 1 })));
	}
	auto& chunkBack = mChunkData.at(key);

	// Set Left
	key = KeyGenerator({ chunkCoords.x - 1, chunkCoords.y });
	if (mChunkData.find(key) == mChunkData.end()) {
		mChunkData.insert(std::make_pair(key, CreateChunkData({ chunkCoords.x - 1, chunkCoords.y })));
	}
	auto& chunkLeft = mChunkData.at(key);

	// Set Right
	key = KeyGenerator({ chunkCoords.x + 1, chunkCoords.y });
	if (mChunkData.find(key) == mChunkData.end()) {
		mChunkData.insert(std::make_pair(key, CreateChunkData({ chunkCoords.x + 1, chunkCoords.y })));
	}
	auto& chunkRight = mChunkData.at(key);

	for (int z = 0; z < 16; ++z) {
		for (int y = 0; y < 32; ++y) {
			data[z + 1][y][17] = chunkFront.blocksData[z][y][0];
			data[z + 1][y][0] = chunkBack.blocksData[z][y][15];
			data[0][y][z + 1] = chunkLeft.blocksData[15][y][z];
			data[17][y][z + 1] = chunkRight.blocksData[0][y][z];
		}
	}

	auto& chunkRenderObjects = mChunkRenderObjects.at(KeyGenerator(chunkCoords));
	mChunkMeshCreator.CreateMeshFromData(chunkCoords, data, chunkRenderObjects);

	//auto start = std::chrono::high_resolution_clock::now();
	//auto stop = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	//duration = duration;
}

ChunkData WorldService::CreateChunkData(Vector2Int chunkCoords)
{
	//switch (BiomeTypes::Forest)
	switch (GetBiomeType(chunkCoords))
	{
	case BiomeTypes::Desert:
		return CreateDesertChunkData(chunkCoords);
	case BiomeTypes::Forest:
		return CreateForestChunkData(chunkCoords);
	case BiomeTypes::Mountains:
		return CreateMountainChunkData(chunkCoords);
	case BiomeTypes::Ocean:
		return CreateOceanChunkData(chunkCoords);
	case BiomeTypes::Plains:
		return CreatePlainsChunkData(chunkCoords);
	case BiomeTypes::SnowyTundra:
		return CreateSnowyTundraChunkData(chunkCoords);
	default:
		return CreatePlainsChunkData(chunkCoords);
	}
}

ChunkData WorldService::CreateDesertChunkData(Vector2Int chunkCoords)
{
	ChunkData chunkData(&mChunkData, chunkCoords);

	// Assign Biome
	chunkData.biomeType = BiomeTypes::Desert;

	bool isCactus[16][16] = { false };
	std::vector<BiomeTypes> biomeType = std::move(GetChunkRuleSet(BiomeTypes::Desert, chunkCoords));

	// Ocean Blending
	const bool nextToOceanRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanUp = GetBiomeType({ chunkCoords.x, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanDown = GetBiomeType({ chunkCoords.x, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOcean = nextToOceanRight || nextToOceanLeft || nextToOceanUp || nextToOceanDown;

	const bool upRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool upLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanAngle = upRight || downRight || upLeft || downLeft;

	// Assign block data
	for (int x = 0; x < mChunkSize.x; ++x)
	{
		for (int z = 0; z < mChunkSize.z; ++z)
		{
			const float perlinCoordX = static_cast<float>(x + (chunkCoords.x * mChunkSize.x));	// * 16	- Width of chunk
			const float perlinCoordY = static_cast<float>(z + (chunkCoords.y * mChunkSize.z));	// * 16 - Width of chunk
			int heightGen = static_cast<int>(mNoise.GetNoise(perlinCoordX, perlinCoordY) * mHeightIntensity + mHeightOffSet);

			if (nextToOcean)
			{
				BlendToOcean(heightGen, Vector2Int(x, z), nextToOceanRight, nextToOceanLeft, nextToOceanUp, nextToOceanDown);
			}
			else if (nextToOceanAngle)
			{
				BlendToOceanAngle(heightGen, Vector2Int(x, z), upRight, downRight, upLeft, downLeft);
			}

			heightGen = Clamp(heightGen, 0, mChunkSize.y - 1);
			DoChunkRuleSet(biomeType[z * mChunkSize.x + x], chunkData, heightGen, 3, x, z);

			// Cactus Pass
			if (heightGen < mChunkSize.y - 2)
			{
				auto noise = mTreeZoneNoise.GetNoise(perlinCoordX, perlinCoordY) * mTreeZoneScale;
				if (noise > mTreeZoneThreshold)
				{
					noise = mTreeNoise.GetNoise(static_cast<float>(x * chunkCoords.y), static_cast<float>(z * chunkCoords.x)) * mTreePlacementScale;

					if (noise > mTreePlacementThreshold)
					{
						bool isCactusClose = false;
						const int minX = x - 1;
						const int maxX = x + 1;
						const int minZ = z - 1;
						const int maxZ = z + 1;

						for (int treeX = minX; treeX < maxX; ++treeX)
						{
							if (isCactusClose)
							{
								break;
							}

							for (int treeZ = minZ; treeZ < maxZ; ++treeZ)
							{
								if (isCactus[treeX][treeZ])
								{
									isCactusClose = true;
									break;
								}
							}
						}

						if (!isCactusClose)
						{
							isCactus[x][z] = true;
							chunkData.blocksData[x][heightGen + 1][z].blockTypes = BlockTypes::Cactus;
							chunkData.blocksData[x][heightGen + 2][z].blockTypes = BlockTypes::Cactus;
						}
					}
				}
			}
		}
	}

	return chunkData;
}

ChunkData WorldService::CreateForestChunkData(Vector2Int chunkCoords)
{
	ChunkData chunkData(&mChunkData, chunkCoords);

	// Assign Biome
	chunkData.biomeType = BiomeTypes::Forest;

	bool isTree[16][16] = { false };
	std::vector<BiomeTypes> biomeType = std::move(GetChunkRuleSet(BiomeTypes::Forest, chunkCoords));

	// Ocean Blending
	const bool nextToOceanRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanUp = GetBiomeType({ chunkCoords.x, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanDown = GetBiomeType({ chunkCoords.x, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOcean = nextToOceanRight || nextToOceanLeft || nextToOceanUp || nextToOceanDown;

	const bool upRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool upLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanAngle = upRight || downRight || upLeft || downLeft;

	// Assign block data
	for (int x = 0; x < mChunkSize.x; ++x)
	{
		for (int z = 0; z < mChunkSize.z; ++z)
		{
			const float perlinCoordX = static_cast<float>(x + (chunkCoords.x * mChunkSize.x));	// * 16	- Width of chunk
			const float perlinCoordY = static_cast<float>(z + (chunkCoords.y * mChunkSize.z));	// * 16 - Width of chunk
			int heightGen = static_cast<int>(mNoise.GetNoise(perlinCoordX, perlinCoordY) * mHeightIntensity + mHeightOffSet);

			if (nextToOcean)
			{
				BlendToOcean(heightGen, Vector2Int(x, z), nextToOceanRight, nextToOceanLeft, nextToOceanUp, nextToOceanDown);
			}
			else if (nextToOceanAngle)
			{
				BlendToOceanAngle(heightGen, Vector2Int(x, z), upRight, downRight, upLeft, downLeft);
			}

			heightGen = Clamp(heightGen, 0, mChunkSize.y - 1);
			const int surfaceNoise = heightGen - static_cast<int>((mSurfaceNoise.GetNoise(perlinCoordY, perlinCoordX)) * 2.0f + 5.5f);

			DoChunkRuleSet(biomeType[z * mChunkSize.x + x], chunkData, heightGen, surfaceNoise, x, z);

			// Tree Pass
			if (heightGen + 5 < mChunkSize.y)
			{
				auto noise = mTreeZoneNoise.GetNoise(perlinCoordX, perlinCoordY) * mTreeZoneScale;
				if (noise > mTreeZoneThreshold)
				{
					noise = mTreeNoise.GetNoise(static_cast<float>(x * chunkCoords.y), static_cast<float>(z * chunkCoords.x)) * mTreePlacementScale * mTreePlacementScale;

					if (noise > mTreePlacementThreshold)
					{
						bool isTreeClose = false;
						const int minX = x - 1;
						const int maxX = x + 1;
						const int minZ = z - 1;
						const int maxZ = z + 1;

						for (int treeX = minX; treeX < maxX; ++treeX)
						{
							if (isTreeClose)
							{
								break;
							}

							for (int treeZ = minZ; treeZ < maxZ; ++treeZ)
							{
								if (isTree[treeX][treeZ])
								{
									isTreeClose = true;
									break;
								}
							}
						}

						if (!isTreeClose)
						{
							isTree[x][z] = true;
							CreateTree(chunkData, { x, heightGen, z });
						}
					}
				}
			}
		}
	}

	return chunkData;
}

ChunkData WorldService::CreateMountainChunkData(Vector2Int chunkCoords)
{
	ChunkData chunkData(&mChunkData, chunkCoords);

	// Assign Biome
	chunkData.biomeType = BiomeTypes::Mountains;

	std::vector<BiomeTypes> biomeType = std::move(GetChunkRuleSet(BiomeTypes::Mountains, chunkCoords));

	// Ocean Blending
	const bool nextToOceanRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanUp = GetBiomeType({ chunkCoords.x, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanDown = GetBiomeType({ chunkCoords.x, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOcean = nextToOceanRight || nextToOceanLeft || nextToOceanUp || nextToOceanDown;

	const bool upRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool upLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanAngle = upRight || downRight || upLeft || downLeft;

	// Assign block data
	for (int x = 0; x < mChunkSize.x; ++x)
	{
		for (int z = 0; z < mChunkSize.z; ++z)
		{
			const float perlinCoordX = static_cast<float>(x + (chunkCoords.x * mChunkSize.x));	// * 16	- Width of chunk
			const float perlinCoordY = static_cast<float>(z + (chunkCoords.y * mChunkSize.z));	// * 16 - Width of chunk
			int heightGen = static_cast<int>(mNoise.GetNoise(perlinCoordX, perlinCoordY) * mHeightIntensity + mHeightOffSet);

			if (nextToOcean)
			{
				BlendToOcean(heightGen, Vector2Int(x, z), nextToOceanRight, nextToOceanLeft, nextToOceanUp, nextToOceanDown);
			}
			else if (nextToOceanAngle)
			{
				BlendToOceanAngle(heightGen, Vector2Int(x, z), upRight, downRight, upLeft, downLeft);
			}

			heightGen = Clamp(heightGen, 0, mChunkSize.y - 1);

			DoChunkRuleSet(biomeType[z * mChunkSize.x + x], chunkData, heightGen, 3, x, z);
		}
	}

	return chunkData;
}

ChunkData WorldService::CreateOceanChunkData(Vector2Int chunkCoords)
{
	ChunkData chunkData(&mChunkData, chunkCoords);

	// Assign Biome
	chunkData.biomeType = BiomeTypes::Ocean;

	const bool nextToOceanRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y }) != BiomeTypes::Ocean;
	const bool nextToOceanLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y }) != BiomeTypes::Ocean;
	const bool nextToOceanUp = GetBiomeType({ chunkCoords.x, chunkCoords.y + 1 }) != BiomeTypes::Ocean;
	const bool nextToOceanDown = GetBiomeType({ chunkCoords.x, chunkCoords.y - 1 }) != BiomeTypes::Ocean;

	const bool upRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y + 1 }) != BiomeTypes::Ocean;
	const bool downRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y - 1 }) != BiomeTypes::Ocean;
	const bool upLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y + 1 }) != BiomeTypes::Ocean;
	const bool downLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y - 1 }) != BiomeTypes::Ocean;

	// Assign block data
	for (int x = 0; x < mChunkSize.x; ++x)
	{
		for (int z = 0; z < mChunkSize.z; ++z)
		{
			float perlinCoordX = static_cast<float>(x + (chunkCoords.x * mChunkSize.x));	// * 16	- Width of chunk
			float perlinCoordY = static_cast<float>(z + (chunkCoords.y * mChunkSize.z));	// * 16 - Width of chunk
			int heightGen = static_cast<int>(mNoise.GetNoise(perlinCoordX, perlinCoordY) * mHeightIntensity + mHeightOffSet * 0.5f);

			int heightGenX = heightGen;
			int heightGenZ = heightGen;

			if (nextToOceanRight || nextToOceanLeft || nextToOceanUp || nextToOceanDown)
			{
				if (nextToOceanRight && nextToOceanLeft)
				{
					if (x > 7)
					{
						// Ocean to Right - chunkCoords.x + 1, chunkCoords.y
						heightGenX = Lerp(heightGen, mOceanHeight + 1, static_cast<float>(x + 1) / static_cast<float>(mChunkSize.x));
					}
					else
					{
						// Ocean to left - chunkCoords.x - 1, chunkCoords.y
						heightGenX = Lerp(mOceanHeight + 2, heightGen, static_cast<float>(x + 1) / static_cast<float>(mChunkSize.x));
					}
				}
				else if (nextToOceanRight)
				{
					// Ocean to Right - chunkCoords.x + 1, chunkCoords.y
					heightGenX = Lerp(heightGen, mOceanHeight + 1, static_cast<float>(x + 1) / static_cast<float>(mChunkSize.x));
				}
				else if (nextToOceanLeft)
				{
					// Ocean to left - chunkCoords.x - 1, chunkCoords.y
					heightGenX = Lerp(mOceanHeight + 2, heightGen, static_cast<float>(x + 1) / static_cast<float>(mChunkSize.x));
				}

				if (nextToOceanUp && nextToOceanDown)
				{
					if (z > 7)
					{
						// Ocean to up - chunkCoords.x, chunkCoords.y + 1
						heightGenZ = Lerp(heightGen, mOceanHeight + 1, static_cast<float>(z + 1) / static_cast<float>(mChunkSize.z));
					}
					else
					{
						// Ocean to down - chunkCoords.x, chunkCoords.y - 1
						heightGenZ = Lerp(mOceanHeight + 2, heightGen, static_cast<float>(z + 1) / static_cast<float>(mChunkSize.z));
					}
				}
				else if (nextToOceanUp)
				{
					// Ocean to up - chunkCoords.x, chunkCoords.y + 1
					heightGenZ = Lerp(heightGen, mOceanHeight + 1, static_cast<float>(z + 1) / static_cast<float>(mChunkSize.z));
				}
				else if (nextToOceanDown)
				{
					// Ocean to down - chunkCoords.x, chunkCoords.y - 1
					heightGenZ = Lerp(mOceanHeight + 2, heightGen, static_cast<float>(z + 1) / static_cast<float>(mChunkSize.z));
				}

				heightGen = heightGen > mOceanHeight ? Min(heightGenX, heightGenZ) : Max(heightGenX, heightGenZ);
			}
			else if (upRight)
			{
				heightGenX = Lerp(heightGen, mOceanHeight + 1, static_cast<float>(x + 1) / static_cast<float>(mChunkSize.x));
				heightGenZ = Lerp(heightGen, mOceanHeight + 1, static_cast<float>(z + 1) / static_cast<float>(mChunkSize.z));

				heightGen = heightGen > mOceanHeight ? Max(heightGenX, heightGenZ) : Min(heightGenX, heightGenZ);
			}
			else if (downRight)
			{
				heightGenX = Lerp(heightGen, mOceanHeight + 1, static_cast<float>(x + 1) / static_cast<float>(mChunkSize.x));
				heightGenZ = Lerp(mOceanHeight + 2, heightGen, static_cast<float>(z + 1) / static_cast<float>(mChunkSize.z));

				heightGen = heightGen > mOceanHeight ? Max(heightGenX, heightGenZ) : Min(heightGenX, heightGenZ);
			}
			else if (upLeft)
			{
				heightGenX = Lerp(mOceanHeight + 2, heightGen, static_cast<float>(x + 1) / static_cast<float>(mChunkSize.x));
				heightGenZ = Lerp(heightGen, mOceanHeight + 1, static_cast<float>(z + 1) / static_cast<float>(mChunkSize.z));

				heightGen = heightGen > mOceanHeight ? Max(heightGenX, heightGenZ) : Min(heightGenX, heightGenZ);
			}
			else if (downLeft)
			{
				heightGenX = Lerp(mOceanHeight + 2, heightGen, static_cast<float>(x + 1) / static_cast<float>(mChunkSize.x));
				heightGenZ = Lerp(mOceanHeight + 2, heightGen, static_cast<float>(z + 1) / static_cast<float>(mChunkSize.z));

				heightGen = heightGen > mOceanHeight ? Max(heightGenX, heightGenZ) : Min(heightGenX, heightGenZ);
			}


			heightGen = Clamp(heightGen, 0, mChunkSize.y - 1);
			BlockTypes blockTypeToAssign = BlockTypes::Air;
			for (int y = mOceanHeight; y > 0; --y)
			{
				// First Pass
				blockTypeToAssign = y > heightGen ? BlockTypes::Water : BlockTypes::Sand;

				// Gravel Pass
				if (y == heightGen)
				{
					auto noise = mTreeZoneNoise.GetNoise(perlinCoordX, perlinCoordY) * mTreeZoneScale * 0.5f;
					if (noise > mTreeZoneThreshold)
					{
						blockTypeToAssign = BlockTypes::Gravel;
					}
				}

				chunkData.blocksData[x][y][z] = BlockInfo(blockTypeToAssign, static_cast<int8_t>(7));
			}

			if (heightGen < 1)
			{
				chunkData.blocksData[x][1][z].blockTypes = BlockTypes::Gravel;
			}
			chunkData.blocksData[x][0][z].blockTypes = BlockTypes::BedRock;
		}
	}

	return chunkData;
}

ChunkData WorldService::CreatePlainsChunkData(Vector2Int chunkCoords)
{
	ChunkData chunkData(&mChunkData, chunkCoords);

	// Assign Biome
	chunkData.biomeType = BiomeTypes::Plains;

	bool isTree[16][16] = { false };
	std::vector<BiomeTypes> biomeType = std::move(GetChunkRuleSet(BiomeTypes::Plains, chunkCoords));

	// Ocean Blending
	const bool nextToOceanRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanUp = GetBiomeType({ chunkCoords.x, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanDown = GetBiomeType({ chunkCoords.x, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOcean = nextToOceanRight || nextToOceanLeft || nextToOceanUp || nextToOceanDown;

	const bool upRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool upLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanAngle = upRight || downRight || upLeft || downLeft;

	// Assign block data
	for (int x = 0; x < mChunkSize.x; ++x)
	{
		for (int z = 0; z < mChunkSize.z; ++z)
		{
			const float perlinCoordX = static_cast<float>(x + (chunkCoords.x * mChunkSize.x));	// * 16	- Width of chunk
			const float perlinCoordY = static_cast<float>(z + (chunkCoords.y * mChunkSize.z));	// * 16 - Width of chunk
			int heightGen = static_cast<int>(mNoise.GetNoise(perlinCoordX, perlinCoordY) * mHeightIntensity + mHeightOffSet);

			if (nextToOcean)
			{
				BlendToOcean(heightGen, Vector2Int(x, z), nextToOceanRight, nextToOceanLeft, nextToOceanUp, nextToOceanDown);
			}
			else if (nextToOceanAngle)
			{
				BlendToOceanAngle(heightGen, Vector2Int(x, z), upRight, downRight, upLeft, downLeft);
			}

			heightGen = Clamp(heightGen, 0, mChunkSize.y - 1);
			const int surfaceNoise = heightGen - static_cast<int>((mSurfaceNoise.GetNoise(perlinCoordY, perlinCoordX)) * 2.0f + 5.5f);

			DoChunkRuleSet(biomeType[z * mChunkSize.x + x], chunkData, heightGen, surfaceNoise, x, z);

			// Tree Pass
			if (heightGen >= 10 && heightGen + 5 < mChunkSize.y)
			{
				auto noise = mTreeZoneNoise.GetNoise(perlinCoordX, perlinCoordY) * mTreeZoneScale;
				if (noise > mTreeZoneThreshold)
				{
					noise = mTreeNoise.GetNoise(static_cast<float>(x * chunkCoords.y), static_cast<float>(z * chunkCoords.x)) * mTreePlacementScale;

					if (noise > mTreePlacementThreshold)
					{
						bool isTreeClose = false;
						const int minX = x - 1;
						const int maxX = x + 1;
						const int minZ = z - 1;
						const int maxZ = z + 1;

						for (int treeX = minX; treeX < maxX; ++treeX)
						{
							if (isTreeClose)
							{
								break;
							}

							for (int treeZ = minZ; treeZ < maxZ; ++treeZ)
							{
								if (isTree[treeX][treeZ])
								{
									isTreeClose = true;
									break;
								}
							}
						}

						if (!isTreeClose)
						{
							isTree[x][z] = true;
							CreateTree(chunkData, { x, heightGen, z });
						}
					}
				}
			}
		}
	}

	return chunkData;
}

ChunkData WorldService::CreateSnowyTundraChunkData(Vector2Int chunkCoords)
{
	ChunkData chunkData(&mChunkData, chunkCoords);

	// Assign Biome
	chunkData.biomeType = BiomeTypes::SnowyTundra;

	std::vector<BiomeTypes> biomeType = std::move(GetChunkRuleSet(BiomeTypes::SnowyTundra, chunkCoords));

	// Ocean Blending
	const bool nextToOceanRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y }) == BiomeTypes::Ocean;
	const bool nextToOceanUp = GetBiomeType({ chunkCoords.x, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanDown = GetBiomeType({ chunkCoords.x, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOcean = nextToOceanRight || nextToOceanLeft || nextToOceanUp || nextToOceanDown;

	const bool upRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downRight = GetBiomeType({ chunkCoords.x + 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool upLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y + 1 }) == BiomeTypes::Ocean;
	const bool downLeft = GetBiomeType({ chunkCoords.x - 1, chunkCoords.y - 1 }) == BiomeTypes::Ocean;
	const bool nextToOceanAngle = upRight || downRight || upLeft || downLeft;

	// Assign block data
	for (int x = 0; x < mChunkSize.x; ++x)
	{
		for (int z = 0; z < mChunkSize.z; ++z)
		{
			const float perlinCoordX = static_cast<float>(x + (chunkCoords.x * mChunkSize.x));
			const float perlinCoordY = static_cast<float>(z + (chunkCoords.y * mChunkSize.z));
			int heightGen = static_cast<int>(mNoise.GetNoise(perlinCoordX, perlinCoordY) * mHeightIntensity + mHeightOffSet);

			if (nextToOcean)
			{
				BlendToOcean(heightGen, Vector2Int(x, z), nextToOceanRight, nextToOceanLeft, nextToOceanUp, nextToOceanDown);
			}
			else if (nextToOceanAngle)
			{
				BlendToOceanAngle(heightGen, Vector2Int(x, z), upRight, downRight, upLeft, downLeft);
			}

			heightGen = Clamp(heightGen, 0, mChunkSize.y - 1);
			const int surfaceNoise = heightGen - static_cast<int>((mSurfaceNoise.GetNoise(perlinCoordY, perlinCoordX)) * 2.0f + 5.5f);

			DoChunkRuleSet(biomeType[z * mChunkSize.x + x], chunkData, heightGen, surfaceNoise, x, z);
		}
	}

	return chunkData;
}

bool WorldService::IsBiomeSuperior(BiomeTypes currBiome, BiomeTypes otherBiome)
{
	return currBiome > otherBiome;
}

std::vector<BiomeTypes> WorldService::GetChunkRuleSet(BiomeTypes biome, SAGE::Math::Vector2Int chunkCoords)
{
	std::vector<BiomeTypes> biomeTypes;
	const int totalSize = mChunkSize.x * mChunkSize.z;
	biomeTypes.reserve(totalSize);

	const BiomeTypes northBiome = GetBiomeType(SAGE::Math::Vector2Int(chunkCoords.x, chunkCoords.y + 1));
	const BiomeTypes eastBiome = GetBiomeType(SAGE::Math::Vector2Int(chunkCoords.x + 1, chunkCoords.y));
	const BiomeTypes southBiome = GetBiomeType(SAGE::Math::Vector2Int(chunkCoords.x, chunkCoords.y - 1));
	const BiomeTypes westBiome = GetBiomeType(SAGE::Math::Vector2Int(chunkCoords.x - 1, chunkCoords.y));

	// Set default
	for (int i = 0; i < totalSize; ++i)
	{
		biomeTypes.push_back(biome);
	}

	// North Blend
	if (northBiome != biome && northBiome != BiomeTypes::Ocean && IsBiomeSuperior(biome, northBiome))
	{
		for (int x = 0; x < mChunkSize.x; ++x)
		{
			const float noise = mBiomeBlendingNoise.GetNoise(static_cast<float>(Max(Abs(chunkCoords.x), 1) * (x + 1)), static_cast<float>(Max(Abs(chunkCoords.y), 1)));
			const int zSize = static_cast<int>(Lerp(0.0f, 5.0f, (noise + 1.0f) / 2.0f) + 0.5f);
			for (int z = mChunkSize.z - 1; z > mChunkSize.z - zSize; --z)
			{
				biomeTypes[z * mChunkSize.x + x] = northBiome;
			}
		}

		// NorthWest
		if (northBiome == westBiome)
		{
			biomeTypes[240] = northBiome;
			biomeTypes[241] = northBiome;
			biomeTypes[242] = northBiome;
			biomeTypes[224] = northBiome;
			biomeTypes[225] = northBiome;
			biomeTypes[208] = northBiome;
		}

		// NorthEast
		if (northBiome == eastBiome)
		{
			biomeTypes[253] = northBiome;
			biomeTypes[254] = northBiome;
			biomeTypes[255] = northBiome;
			biomeTypes[238] = northBiome;
			biomeTypes[239] = northBiome;
			biomeTypes[223] = northBiome;
		}
	}

	// South Blend
	if (southBiome != biome && southBiome != BiomeTypes::Ocean && IsBiomeSuperior(biome, southBiome))
	{
		for (int x = 0; x < mChunkSize.x; ++x)
		{
			const float noise = mBiomeBlendingNoise.GetNoise(static_cast<float>(Max(Abs(chunkCoords.x), 1) * (x + 1)), static_cast<float>(Max(Abs(chunkCoords.y), 1)));
			const int zSize = static_cast<int>(Lerp(0.0f, 5.0f, (noise + 1.0f) / 2.0f) + 0.5f);
			for (int z = 0; z < zSize; ++z)
			{
				biomeTypes[z * mChunkSize.x + x] = southBiome;
			}
		}

		// SouthWest
		if (southBiome == westBiome)
		{
			biomeTypes[0] = southBiome;
			biomeTypes[1] = southBiome;
			biomeTypes[2] = southBiome;
			biomeTypes[16] = southBiome;
			biomeTypes[17] = southBiome;
			biomeTypes[32] = southBiome;
		}

		// SouthEast
		if (southBiome == eastBiome)
		{
			biomeTypes[13] = southBiome;
			biomeTypes[14] = southBiome;
			biomeTypes[15] = southBiome;
			biomeTypes[30] = southBiome;
			biomeTypes[31] = southBiome;
			biomeTypes[47] = southBiome;
		}
	}

	// West Blend
	if (westBiome != biome && westBiome != BiomeTypes::Ocean && IsBiomeSuperior(biome, westBiome))
	{
		for (int z = 0; z < mChunkSize.z; ++z)
		{
			const float noise = mBiomeBlendingNoise.GetNoise(static_cast<float>(Max(Abs(chunkCoords.x), 1)), static_cast<float>(Max(Abs(chunkCoords.y), 1) * (z + 1)));
			const int xSize = static_cast<int>(Lerp(0.0f, 5.0f, (noise + 1.0f) / 2.0f) + 0.5f);
			for (int x = 0; x < xSize; ++x)
			{
				biomeTypes[z * mChunkSize.x + x] = westBiome;
			}
		}
	}

	// East Blend
	if (eastBiome != biome && eastBiome != BiomeTypes::Ocean && IsBiomeSuperior(biome, eastBiome))
	{
		for (int z = 0; z < mChunkSize.z; ++z)
		{
			const float noise = mBiomeBlendingNoise.GetNoise(static_cast<float>(Max(Abs(chunkCoords.x), 1)), static_cast<float>(Max(Abs(chunkCoords.y), 1) * (z + 1)));
			const int xSize = static_cast<int>(Lerp(0.0f, 5.0f, (noise + 1.0f) / 2.0f) + 0.5f);
			for (int x = mChunkSize.x - 1; x > mChunkSize.x - xSize; --x)
			{
				biomeTypes[z * mChunkSize.x + x] = eastBiome;
			}
		}
	}

	return biomeTypes;
}

void WorldService::DoChunkRuleSet(BiomeTypes biomeType, ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z)
{
	switch (biomeType)
	{
	case BiomeTypes::Desert:
		DesertRuleSet(chunkData, heightGen, x, z);
		break;
	case BiomeTypes::Forest:
		ForestRuleSet(chunkData, heightGen, surfaceNoise, x, z);
		break;
	case BiomeTypes::Mountains:
	{
		const float perlinCoordX = static_cast<float>(x + (chunkData.chunkCoords.x * mChunkSize.x));
		const float perlinCoordY = static_cast<float>(z + (chunkData.chunkCoords.y * mChunkSize.z));
		MountainRuleSet(chunkData, heightGen, Vector2(perlinCoordX, perlinCoordY), x, z);
	}
	break;
	case BiomeTypes::Ocean:
		OceanRuleSet(chunkData, heightGen, surfaceNoise, x, z);
		break;
	case BiomeTypes::SnowyTundra:
		SnowyTundraRuleSet(chunkData, heightGen, surfaceNoise, x, z);
		break;
	default:
		PlainRuleSet(chunkData, heightGen, surfaceNoise, x, z);
		break;
	}
}

void WorldService::DesertRuleSet(ChunkData& chunkData, int heightGen, int x, int z)
{
	for (int y = heightGen; y > 0; --y)
	{
		chunkData.blocksData[x][y][z].blockTypes = BlockTypes::Sand;
	}
	chunkData.blocksData[x][0][z].blockTypes = BlockTypes::BedRock;
}

void WorldService::ForestRuleSet(ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z)
{
	for (int y = heightGen - 1; y > 0; --y)
	{
		chunkData.blocksData[x][y][z].blockTypes = y > surfaceNoise ? BlockTypes::Dirt : BlockTypes::Stone;
	}

	// Set first layer to grass
	chunkData.blocksData[x][heightGen][z].blockTypes = BlockTypes::GrassBlock;
	chunkData.blocksData[x][0][z].blockTypes = BlockTypes::BedRock;
}

void WorldService::MountainRuleSet(ChunkData& chunkData, int heightGen, Vector2 perlin, int x, int z)
{
	BlockTypes blockTypeToAssign = BlockTypes::Air;
	for (int y = heightGen; y > 0; --y)
	{
		// First Pass
		blockTypeToAssign = BlockTypes::Stone;

		// GrassBlock Pass
		if (y == heightGen)
		{
			auto noise = mTreeZoneNoise.GetNoise(perlin.x, perlin.y) * mTreeZoneScale * 0.35f;
			if (noise > mTreeZoneThreshold)
			{
				blockTypeToAssign = BlockTypes::GrassBlock;
			}
		}

		// CoalOre Pass
		if (y == heightGen)
		{
			auto noise = mTreeZoneNoise.GetNoise(perlin.x, perlin.y) * mTreeZoneScale * 0.22f;
			if (noise > mTreeZoneThreshold)
			{
				blockTypeToAssign = BlockTypes::CoalOre;
			}
		}

		chunkData.blocksData[x][y][z].blockTypes = blockTypeToAssign;
	}
	chunkData.blocksData[x][0][z].blockTypes = BlockTypes::BedRock;
}

void WorldService::OceanRuleSet(ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z)
{
	// TODO: Beach?
}

void WorldService::PlainRuleSet(ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z)
{
	BlockTypes blockTypeToAssign = BlockTypes::Air;

	//heightGen = 25;
	for (int y = heightGen - 1; y > 0; --y)
	{
		// Set next 3 layers to dirt
		if (heightGen < 9 && y < heightGen && y > surfaceNoise)
		{
			blockTypeToAssign = BlockTypes::Sand;
		}
		else if (y < heightGen && y > surfaceNoise)
		{
			blockTypeToAssign = BlockTypes::Dirt;
		}

		// Set everything between dirt range (inclusive) and 0 (exclusive) to stone
		else if (y <= surfaceNoise && y > 0)
		{
			blockTypeToAssign = BlockTypes::Stone;

			// Coal
			//float perlinX = static_cast<float>(x + (chunkCoords.x * mChunkSize.x));
			//float perlinY = static_cast<float>(y + (chunkCoords.y * mChunkSize.y));
			//float perlinZ = static_cast<float>(z + (chunkCoords.y * mChunkSize.z));
			//const float caveNoise = abs(mCoalNoise.GetNoise(perlinX, perlinY, perlinZ));

			//blockTypeToAssign = caveNoise > 0.825f ? BlockTypes::CoalOre : BlockTypes::Stone;
		}

		// Caves
		//if (blockTypeToAssign != BlockTypes::BedRock)
		//{
		//	float perlinX = static_cast<float>(x + (chunkCoords.x * mChunkSize[0]));
		//	float perlinY = static_cast<float>(y + (chunkCoords.y * mChunkSize[1]));
		//	float perlinZ = static_cast<float>(z + (chunkCoords.x * mChunkSize[2]));
		//	const float caveNoise = abs(mNoise.GetNoise(perlinX, perlinY, perlinZ)) * 10;
		//	if (caveNoise < 0.25f)
		//	{
		//		blockTypeToAssign = BlockTypes::Air;
		//	}
		//}

		chunkData.blocksData[x][y][z].blockTypes = blockTypeToAssign;
	}

	// Set first layer to grass
	if (heightGen < 10)
	{
		chunkData.blocksData[x][heightGen][z].blockTypes = BlockTypes::Sand;
		chunkData.blocksData[x][heightGen + 1][z] = std::move(BlockInfo(BlockTypes::Water, 7));
	}
	else
	{
		chunkData.blocksData[x][heightGen][z].blockTypes = BlockTypes::GrassBlock;
	}

	chunkData.blocksData[x][0][z].blockTypes = BlockTypes::BedRock;
}

void WorldService::SnowyTundraRuleSet(ChunkData& chunkData, int heightGen, int surfaceNoise, int x, int z)
{
	BlockTypes blockTypeToAssign = BlockTypes::Air;
	for (int y = heightGen - 1; y > 0; --y)
	{
		// Set next 3 layers to dirt
		if (y < heightGen && y > surfaceNoise)
		{
			blockTypeToAssign = BlockTypes::Dirt;
		}
		else if (y <= surfaceNoise && y > 0) // Set everything between dirt range (inclusive) and 0 (exclusive) to stone
		{
			blockTypeToAssign = BlockTypes::Stone;
		}

		chunkData.blocksData[x][y][z].blockTypes = blockTypeToAssign;
	}
	chunkData.blocksData[x][heightGen][z].blockTypes = BlockTypes::SnowGrassBlock;
	chunkData.blocksData[x][0][z].blockTypes = BlockTypes::BedRock;
}

void WorldService::CreateTree(ChunkData& chunkData, Vector3Int basePosition)
{
	const int minX = basePosition.x - 1;
	const int midX = basePosition.x + 1;
	const int maxX = midX + 1;
	const int minY = basePosition.y + Random::UniformInt(2, 3);
	const int midY = basePosition.y + 3 + 1;
	const int maxY = midY + 1;
	const int minZ = basePosition.z - 1;
	const int midZ = basePosition.z + 1;
	const int maxZ = midZ + 1;

	// Leaves
	for (int y = minY; y < maxY; ++y)
	{
		for (int x = minX; x < maxX; ++x)
		{
			for (int z = minZ; z < maxZ; ++z)
			{
				if (y == maxY - 1 && ((x == minX && z == minZ) || (x == midX && z == minZ)
					|| (x == minX && z == midZ) || (x == midX && z == midZ)) && Random::UniformFloat() < 0.5f)
				{
					continue;
				}

				if (x >= 0 && x < mChunkSize.x && z >= 0 && z < mChunkSize.z)
				{
					chunkData.blocksData[x][y][z].blockTypes = BlockTypes::OakLeaves;
				}
				else
				{
					auto coord = chunkData.chunkCoords;
					int xPos = x;
					int zPos = z;

					if (x < 0)
					{
						--coord.x;
						xPos = mChunkSize.x + x;
					}
					else if (x >= mChunkSize.x)
					{
						++coord.x;
						xPos = x - mChunkSize.x;
					}

					if (z < 0)
					{
						--coord.y;
						zPos = mChunkSize.z + z;
					}
					else if (z >= mChunkSize.z)
					{
						++coord.y;
						zPos = z - mChunkSize.z;
					}

					AddChunkModification(coord, BlockInfo(BlockTypes::OakLeaves), Vector3Int(xPos, y, zPos));
				}
			}
		}
	}

	// Stem
	chunkData.blocksData[basePosition.x][basePosition.y + 1][basePosition.z].blockTypes = BlockTypes::OakLog;
	chunkData.blocksData[basePosition.x][basePosition.y + 2][basePosition.z].blockTypes = BlockTypes::OakLog;
	chunkData.blocksData[basePosition.x][basePosition.y + 3][basePosition.z].blockTypes = BlockTypes::OakLog;
	chunkData.blocksData[basePosition.x][basePosition.y + 4][basePosition.z].blockTypes = BlockTypes::OakLog;
	chunkData.blocksData[basePosition.x][basePosition.y + 5][basePosition.z].blockTypes = BlockTypes::OakLeaves;
}

void WorldService::BlendToOcean(int& heightGen, Vector2Int blockPos, bool nextToOceanRight, bool nextToOceanLeft, bool nextToOceanUp, bool nextToOceanDown)
{
	int heightGenX = heightGen;
	int heightGenZ = heightGen;

	if (nextToOceanRight && nextToOceanLeft)
	{
		if (blockPos.x > 7)
		{
			// Ocean to Right - chunkCoords.x + 1, chunkCoords.y
			heightGenX = Lerp(heightGen, mOceanHeight, static_cast<float>(blockPos.x + 1) / static_cast<float>(mChunkSize.x));
		}
		else
		{
			// Ocean to left - chunkCoords.x - 1, chunkCoords.y
			heightGenX = Lerp(mOceanHeight + 1, heightGen, static_cast<float>(blockPos.x + 1) / static_cast<float>(mChunkSize.x));
		}
	}
	else if (nextToOceanRight)
	{
		// Ocean to Right - chunkCoords.x + 1, chunkCoords.y
		heightGenX = Lerp(heightGen, mOceanHeight, static_cast<float>(blockPos.x + 1) / static_cast<float>(mChunkSize.x));
	}
	else if (nextToOceanLeft)
	{
		// Ocean to left - chunkCoords.x - 1, chunkCoords.y
		heightGenX = Lerp(mOceanHeight + 1, heightGen, static_cast<float>(blockPos.x + 1) / static_cast<float>(mChunkSize.x));
	}

	if (nextToOceanUp && nextToOceanDown)
	{
		if (blockPos.y > 7)
		{
			// Ocean to up - chunkCoords.x, chunkCoords.y + 1
			heightGenZ = Lerp(heightGen, mOceanHeight, static_cast<float>(blockPos.y + 1) / static_cast<float>(mChunkSize.z));
		}
		else
		{
			// Ocean to down - chunkCoords.x, chunkCoords.y - 1
			heightGenZ = Lerp(mOceanHeight + 1, heightGen, static_cast<float>(blockPos.y + 1) / static_cast<float>(mChunkSize.z));
		}
	}
	else if (nextToOceanUp)
	{
		// Ocean to up - chunkCoords.x, chunkCoords.y + 1
		heightGenZ = Lerp(heightGen, mOceanHeight, static_cast<float>(blockPos.y + 1) / static_cast<float>(mChunkSize.z));
	}
	else if (nextToOceanDown)
	{
		// Ocean to down - chunkCoords.x, chunkCoords.y - 1
		heightGenZ = Lerp(mOceanHeight + 1, heightGen, static_cast<float>(blockPos.y + 1) / static_cast<float>(mChunkSize.z));
	}

	heightGen = heightGen > mOceanHeight ? Min(heightGenX, heightGenZ) : Max(heightGenX, heightGenZ);
}

void WorldService::BlendToOceanAngle(int& heightGen, SAGE::Math::Vector2Int blockPos, bool upRight, bool downRight, bool upLeft, bool downLeft)
{
	int heightGenX = heightGen;
	int heightGenZ = heightGen;

	if (upRight)
	{
		heightGenX = Lerp(heightGen, mOceanHeight, static_cast<float>(blockPos.x + 1) / static_cast<float>(mChunkSize.x));
		heightGenZ = Lerp(heightGen, mOceanHeight, static_cast<float>(blockPos.y + 1) / static_cast<float>(mChunkSize.z));

		heightGen = heightGen > mOceanHeight ? Max(heightGenX, heightGenZ) : Min(heightGenX, heightGenZ);
	}
	else if (downRight)
	{
		heightGenX = Lerp(heightGen, mOceanHeight, static_cast<float>(blockPos.x + 1) / static_cast<float>(mChunkSize.x));
		heightGenZ = Lerp(mOceanHeight + 1, heightGen, static_cast<float>(blockPos.y + 1) / static_cast<float>(mChunkSize.z));

		heightGen = heightGen > mOceanHeight ? Max(heightGenX, heightGenZ) : Min(heightGenX, heightGenZ);
	}
	else if (upLeft)
	{
		heightGenX = Lerp(mOceanHeight + 1, heightGen, static_cast<float>(blockPos.x + 1) / static_cast<float>(mChunkSize.x));
		heightGenZ = Lerp(heightGen, mOceanHeight, static_cast<float>(blockPos.y + 1) / static_cast<float>(mChunkSize.z));

		heightGen = heightGen > mOceanHeight ? Max(heightGenX, heightGenZ) : Min(heightGenX, heightGenZ);
	}
	else if (downLeft)
	{
		heightGenX = Lerp(mOceanHeight + 1, heightGen, static_cast<float>(blockPos.x + 1) / static_cast<float>(mChunkSize.x));
		heightGenZ = Lerp(mOceanHeight + 1, heightGen, static_cast<float>(blockPos.y + 1) / static_cast<float>(mChunkSize.z));

		heightGen = heightGen > mOceanHeight ? Max(heightGenX, heightGenZ) : Min(heightGenX, heightGenZ);
	}
}

void WorldService::AddChunkModification(Vector2Int chunkCoords, BlockInfo blockInfo, Vector3Int position)
{
	// Check to see if there is an already open modification
	const std::string chunkDataKey = KeyGenerator(chunkCoords);
	if (mChunkModifications.find(chunkDataKey) != mChunkModifications.end())
	{
		auto& chunkModification = mChunkModifications.at(chunkDataKey);
		chunkModification.mBlockModifications.push_back(BlockModification(blockInfo, position));
	}
	else // Make One
	{
		ChunkModification chunkModification;
		chunkModification.mBlockModifications.push_back(BlockModification(blockInfo, position));
		mChunkModifications.insert(std::make_pair(chunkDataKey, chunkModification));
	}
}

void WorldService::ModifyChunk(Vector2Int chunkCoords)
{
	const std::string key = KeyGenerator(chunkCoords);
	if (mChunkModifications.find(key) == mChunkModifications.end()) { return; }
	if (mChunkData.find(key) == mChunkData.end()) { return; }

	auto& chunkModification = mChunkModifications.at(key);
	auto& chunkData = mChunkData.at(key);

	// Modify
	for (const auto& blockData : chunkModification.mBlockModifications)
	{
		chunkData.SetBlock(blockData.position, blockData.blockInfo);
	}

	// Remove from modifications
	mChunkModifications.erase(key);

	// Update chunk 
	UpdateChunk(chunkCoords);
	UpdateAABBS();
}

void WorldService::UpdateChunk(const Vector2Int& chunkCoords)
{
	const std::string chunkDataKey = KeyGenerator(chunkCoords);
	if (mChunkRenderObjects.find(chunkDataKey) != mChunkRenderObjects.end()) {
		auto& chunkRenderObject = mChunkRenderObjects.at(chunkDataKey);
		chunkRenderObject.opaqueChunkRenderObject.meshBuffer.Terminate();
		chunkRenderObject.transparentChunkRenderObject.meshBuffer.Terminate();
		chunkRenderObject.waterChunkRenderObject.meshBuffer.Terminate();
		InitializeMesh(chunkCoords, mChunkData.at(chunkDataKey));
		chunkRenderObject.RemoveChunkCollider();
		chunkRenderObject.CreateChunkCollider();
	}
}

void WorldService::UpdateAABBS()
{
	mAABBs.clear();

	const int size = 1; // 1
	for (int x = mNewWorldCoords.x - size; x <= mNewWorldCoords.x + size; ++x)
	{
		for (int y = mNewWorldCoords.y - size; y <= mNewWorldCoords.y + size; ++y)
		{
			const std::string key = KeyGenerator({ x, y });
			if (mChunkData.find(key) != mChunkData.end()) {
				if (mChunkRenderObjects.find(key) != mChunkRenderObjects.end()) {
					const auto& blocks = mChunkRenderObjects.at(key).activeBlocks;
					for (auto& vec : blocks)
					{
						mAABBs.push_back({ vec, {0.5f, 0.5f, 0.5f} });
					}
				}
			}
		}
	}
}

void WorldService::AssignReloadWall(ChunkWall unloadchunkWall)
{
	// Info
	int unloadAxis = 0;
	int loadAxis = 0;
	bool isHor = false;
	switch (unloadchunkWall)
	{
	case ChunkWall::North: // Moving South
		unloadAxis = mOldWorldCoords.y + mRenderDistance;
		loadAxis = mOldWorldCoords.y - mRenderDistance - 1;
		isHor = false;
		break;
	case ChunkWall::East:	// Moving West
		unloadAxis = mOldWorldCoords.x + mRenderDistance;
		loadAxis = mOldWorldCoords.x - mRenderDistance - 1;
		isHor = true;
		break;
	case ChunkWall::South:	// Moving North
		unloadAxis = mOldWorldCoords.y - mRenderDistance;
		loadAxis = mOldWorldCoords.y + mRenderDistance + 1;
		isHor = false;
		break;
	case ChunkWall::West:	// Moving East
		unloadAxis = mOldWorldCoords.x - mRenderDistance;
		loadAxis = mOldWorldCoords.x + mRenderDistance + 1;
		isHor = true;
		break;
	}

	for (int i = -mRenderDistance; i <= mRenderDistance; ++i)
	{
		const int unloadChunkX = isHor ? unloadAxis : mOldWorldCoords.x + i;
		const int unloadChunkZ = isHor ? mOldWorldCoords.y + i : unloadAxis;
		const std::string unloadChunkKey = KeyGenerator({ unloadChunkX, unloadChunkZ });

		if (mChunkRenderObjects.find(unloadChunkKey) != mChunkRenderObjects.end()) {
			const int loadChunkX = isHor ? loadAxis : mOldWorldCoords.x + i;
			const int loadChunkZ = isHor ? mOldWorldCoords.y + i : loadAxis;

			ChunkToReload chunkToReload;
			chunkToReload.oldCoords = { unloadChunkX, unloadChunkZ };
			chunkToReload.newCoords = { loadChunkX, loadChunkZ };
			mChunksToReload.push(chunkToReload);
		}
	}
}

void WorldService::RefreshChunks()
{
	for (int x = mNewWorldCoords.x - mRenderDistance; x <= mNewWorldCoords.x + mRenderDistance; ++x)
	{
		for (int z = mNewWorldCoords.y - mRenderDistance; z <= mNewWorldCoords.y + mRenderDistance; ++z)
		{
			const std::string key = KeyGenerator({ x, z });

			// Create chunkData if it doesnt exist
			if (mChunkData.find(key) == mChunkData.end()) {
				mChunkData.insert(std::make_pair(key, CreateChunkData({ x, z })));
			}

			// Create chunkRenderObject if it doesnt exist
			if (mChunkRenderObjects.find(key) == mChunkRenderObjects.end()) {
				CreateChunkRenderObject(key);
			}

			auto& chunk = mChunkRenderObjects.at(key);

			// Terminate meshBuffers
			chunk.opaqueChunkRenderObject.meshBuffer.Terminate();
			chunk.transparentChunkRenderObject.meshBuffer.Terminate();
			chunk.waterChunkRenderObject.meshBuffer.Terminate();

			// Initialize meshbuffer
			InitializeMesh({ x, z }, mChunkData.at(key));

			// Add rigid body
			chunk.RemoveChunkCollider();
			chunk.CreateChunkCollider();
		}
	}
}

void WorldService::CreateChunkRenderObject(const std::string key)
{
	ChunkRenderer chunkRenderObject(GetWorld().GetService<BPhysicsService>());
	mChunkRenderObjects.insert(std::make_pair(key, chunkRenderObject));
}

void WorldService::ReloadChunk(ChunkToReload chunkToReload)
{
	const std::string unloadChunkKey = KeyGenerator(chunkToReload.oldCoords);
	auto& chunkRenderObject = mChunkRenderObjects.at(unloadChunkKey);
	mRigidBodiesToRemove.push(chunkRenderObject.chunkRB); // Remove rigid body 
	chunkRenderObject.activeBlocks.clear();
	chunkRenderObject.opaqueChunkRenderObject.meshBuffer.Terminate();
	chunkRenderObject.transparentChunkRenderObject.meshBuffer.Terminate();
	chunkRenderObject.waterChunkRenderObject.meshBuffer.Terminate();

	const std::string loadChunkKey = KeyGenerator(chunkToReload.newCoords);
	mChunkRenderObjects.insert(std::make_pair(loadChunkKey, std::move(chunkRenderObject)));
	mChunkRenderObjects.erase(unloadChunkKey);

	InitializeMesh(chunkToReload.newCoords, mChunkData.at(loadChunkKey));

	if (mLiftChunk)
	{
		CoroutineSystem::Get()->StartCoroutine(LiftChunk(loadChunkKey));
	}

	// Add rigid body
	auto& loadChunkRenderObject = mChunkRenderObjects.at(loadChunkKey);
	loadChunkRenderObject.CreateChunkCollider();
}

void WorldService::RemoveOldChunkRigidBodies()
{
	auto physics = GetWorld().GetService<BPhysicsService>();
	while (!mRigidBodiesToRemove.empty())
	{
		if (mRigidBodiesToRemove.front() != nullptr)
		{
			physics->GetDynamicsWorld()->removeRigidBody(mRigidBodiesToRemove.front());
		}

		if (mRigidBodiesToRemove.front() != nullptr && mRigidBodiesToRemove.front()->getMotionState())
		{
			delete mRigidBodiesToRemove.front()->getMotionState();
		}

		SafeDelete(mRigidBodiesToRemove.front());
		mRigidBodiesToRemove.pop();
	}
}


void WorldService::UpdateRenderDistance()
{
	mNewRenderDistance = Clamp(mNewRenderDistance, 1, 15);
	if (mNewRenderDistance < mRenderDistance)
	{
		// Remove no longer needed chunkRenderObjects
		for (auto it = mChunkRenderObjects.cbegin(); it != mChunkRenderObjects.cend();)
		{
			bool mustDelete = true;

			for (int x = mNewWorldCoords.x - mNewRenderDistance; x <= mNewWorldCoords.x + mNewRenderDistance; ++x) {
				for (int z = mNewWorldCoords.y - mNewRenderDistance; z <= mNewWorldCoords.y + mNewRenderDistance; ++z) {
					const std::string key = KeyGenerator({ x, z });
					if (it->first == key)
					{
						mustDelete = false;
						break;
					}
				}

				if (!mustDelete) { break; }
			}

			if (mustDelete) {
				mRigidBodiesToRemove.push(it->second.chunkRB);
				mChunkRenderObjects.erase(it++);
			}
			else {
				++it;
			}
		}
	}
	mRenderDistance = mNewRenderDistance;
	UpdateFogAmount();

	RefreshChunks();
}

void WorldService::UpdateFogAmount()
{
	//const float minSize = mRenderDistance * mChunkSize.x - (mRenderDistance < 4 ? mChunkSize.x / (5 - mRenderDistance) : mChunkSize.x);
	//mVoxelTerrainEffect.UseFog(mRenderDistance >= 5);

	if (mRenderDistance <= 3)
	{
		mUseFog = false;
		mVoxelTerrainEffect.UseFog(mUseFog);
		mVoxelWaterEffect.UseFog(mUseFog);
	}

	const float minSize = static_cast<float>(mRenderDistance * mChunkSize.x - mChunkSize.x);
	const float maxSize = static_cast<float>(mRenderDistance * mChunkSize.x);
	mVoxelTerrainEffect.SetFogStart(minSize);
	mVoxelTerrainEffect.SetFogEnd(maxSize);
	mVoxelWaterEffect.SetFogStart(minSize);
	mVoxelWaterEffect.SetFogEnd(maxSize);
}

Enumerator WorldService::LiftChunk(const std::string loadChunkKey)
{
	return [=](CoroPush& yield_return)
	{
		if (mChunkRenderObjects.find(loadChunkKey) != mChunkRenderObjects.end())
		{
			auto& chunkObj = mChunkRenderObjects.at(loadChunkKey);
			const float minY = -10.0f;
			chunkObj.opaqueChunkRenderObject.transform.position.y = minY;
			chunkObj.transparentChunkRenderObject.transform.position.y = minY;
			chunkObj.waterChunkRenderObject.transform.position.y = minY;

			const auto start = std::chrono::system_clock::now();
			float time = 0.0f;
			const float totalTime = 1.0f;

			while (time < totalTime)
			{
				const auto end = std::chrono::system_clock::now();
				const std::chrono::duration<float> elapsedTime = end - start;
				time = elapsedTime.count();

				const float easeOutSine = sin((time / totalTime * Constants::Pi) * 0.5f);;
				const float riseAmount = Lerp(minY, 0.0f, easeOutSine);

				chunkObj.opaqueChunkRenderObject.transform.position.y = riseAmount;
				chunkObj.transparentChunkRenderObject.transform.position.y = riseAmount;
				chunkObj.waterChunkRenderObject.transform.position.y = riseAmount;

				yield_return(nullptr);
			}
			chunkObj.opaqueChunkRenderObject.transform.position.y = 0.0f;
			chunkObj.transparentChunkRenderObject.transform.position.y = 0.0f;
			chunkObj.waterChunkRenderObject.transform.position.y = 0.0f;
		}
	};
}

// Tick
#pragma region ---Tick---

void WorldService::Tick(float deltaTime)
{
	mTickTimer += deltaTime;
	while (mTickTimer >= mTickStep)
	{
		mTickTimer -= mTickStep;
		const int tickRadius = 1;

		// Block Updates
		for (int y = -tickRadius; y <= tickRadius; ++y)
		{
			for (int x = -tickRadius; x <= tickRadius; ++x)
			{
				ChunkTickUpdate({ mNewWorldCoords.x + x, mNewWorldCoords.y + y });
			}
		}

		// Modifications
		const int minX = mNewWorldCoords.x - mRenderDistance;
		const int maxX = mNewWorldCoords.x + mRenderDistance;
		const int minZ = mNewWorldCoords.y - mRenderDistance;
		const int maxZ = mNewWorldCoords.y + mRenderDistance;

		for (int x = minX; x <= maxX; ++x) {
			for (int z = minZ; z <= maxZ; ++z) {
				ModifyChunk(Vector2Int(x, z));
			}
		}
	}
}

void WorldService::ChunkTickUpdate(SAGE::Math::Vector2Int chunkCoord)
{
	//return;
	const std::string key = KeyGenerator(chunkCoord);
	if (mChunkData.find(key) != mChunkData.end() && mChunkRenderObjects.find(key) != mChunkRenderObjects.end()) {

		auto& chunkData = mChunkData.at(key);
		bool updatedChunkData = false;

		const auto& renderObject = mChunkRenderObjects.at(key);
		for (const auto& blockIndex : renderObject.activeBlocksIndexs)
		{
			const auto blockType = chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].blockTypes;
			switch (blockType)
			{
			case BlockTypes::Dirt: DirtTick(chunkData, blockIndex, updatedChunkData); break;
			case BlockTypes::Sand: SandGravelTick(chunkData, blockIndex, updatedChunkData, blockType); break;
			case BlockTypes::Gravel: SandGravelTick(chunkData, blockIndex, updatedChunkData, blockType); break;
			case BlockTypes::Water: WaterTick(chunkData, blockIndex, updatedChunkData); break;
			case BlockTypes::SugarCane: SugarCaneTick(chunkData, blockIndex, updatedChunkData); break;
			case BlockTypes::OakLeaves: OakLeavesTick(chunkData, blockIndex, updatedChunkData); break;
			case BlockTypes::Cactus: CactusTick(chunkData, blockIndex, updatedChunkData); break;
			case BlockTypes::Wheat: WheatTick(chunkData, blockIndex, updatedChunkData); break;
			}
		}

		if (updatedChunkData)
		{
			UpdateChunk(chunkCoord);
			UpdateAABBS();
		}
	}
}

void WorldService::DirtTick(ChunkData& chunkData, const Vector3Int& blockIndex, bool& changedMesh)
{
	if (chunkData.blocksData[blockIndex.x][blockIndex.y + 1][blockIndex.z].blockTypes != BlockTypes::Air)
	{
		return;
	}

	const BlockTypes GrassType = chunkData.biomeType == BiomeTypes::SnowyTundra ? BlockTypes::SnowGrassBlock : BlockTypes::GrassBlock;

	if (chunkData.blocksData[blockIndex.x + 1][blockIndex.y][blockIndex.z].blockTypes == GrassType || chunkData.blocksData[blockIndex.x - 1][blockIndex.y][blockIndex.z].blockTypes == GrassType ||
		chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z + 1].blockTypes == GrassType || chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z - 1].blockTypes == GrassType)
	{
		if (Random::UniformFloat() < 0.1f)
		{
			chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].blockTypes = GrassType;
			changedMesh = true;
		}
	}
	else if (Random::UniformFloat() < 0.05f)
	{
		chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].blockTypes = GrassType;
		changedMesh = true;
	}
}

void WorldService::SandGravelTick(ChunkData& chunkData, const Vector3Int& blockIndex, bool& changedMesh, const BlockTypes blockType)
{
	if (blockIndex.y == 0 || chunkData.blocksData[blockIndex.x][blockIndex.y - 1][blockIndex.z].blockTypes != BlockTypes::Air)
	{
		return;
	}

	chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].blockTypes = BlockTypes::Air;
	chunkData.blocksData[blockIndex.x][blockIndex.y - 1][blockIndex.z].blockTypes = blockType;
	changedMesh = true;
}

void WorldService::WaterTick(ChunkData& chunkData, const Vector3Int& blockIndex, bool& changedMesh)
{
	BlockInfo& blockInfo = chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z];
	BlockInfo& underBlockInfo = chunkData.blocksData[blockIndex.x][blockIndex.y - 1][blockIndex.z];
	BlockInfo& aboveBlockInfo = chunkData.blocksData[blockIndex.x][blockIndex.y + 1][blockIndex.z];

	Vector3Int northCord = Vector3Int(blockIndex.x, blockIndex.y, blockIndex.z + 1);
	Vector3Int eastCord = Vector3Int(blockIndex.x + 1, blockIndex.y, blockIndex.z);
	Vector3Int southCord = Vector3Int(blockIndex.x, blockIndex.y, blockIndex.z - 1);
	Vector3Int westCord = Vector3Int(blockIndex.x - 1, blockIndex.y, blockIndex.z);
	BlockInfo& northBlockInfo = chunkData.GetBlockInfo(northCord);
	BlockInfo& eastBlockInfo = chunkData.GetBlockInfo(eastCord);
	BlockInfo& southBlockInfo = chunkData.GetBlockInfo(southCord);
	BlockInfo& westBlockInfo = chunkData.GetBlockInfo(westCord);

	// Adjust water amount
	if (blockInfo.padding < static_cast<int8_t>(7))
	{
		if ((northBlockInfo.blockTypes == BlockTypes::Water && northBlockInfo.padding == 7 && eastBlockInfo.blockTypes == BlockTypes::Water && eastBlockInfo.padding == 7)
			|| (eastBlockInfo.blockTypes == BlockTypes::Water && eastBlockInfo.padding == 7 && southBlockInfo.blockTypes == BlockTypes::Water && southBlockInfo.padding == 7)
			|| (southBlockInfo.blockTypes == BlockTypes::Water && southBlockInfo.padding == 7 && westBlockInfo.blockTypes == BlockTypes::Water && westBlockInfo.padding == 7)
			|| (westBlockInfo.blockTypes == BlockTypes::Water && westBlockInfo.padding == 7 && northBlockInfo.blockTypes == BlockTypes::Water && northBlockInfo.padding == 7))
		{
			blockInfo.padding = static_cast<int8_t>(7);
			changedMesh = true;
		}
		else
		{
			if (aboveBlockInfo.blockTypes == BlockTypes::Water && aboveBlockInfo.padding > blockInfo.padding + static_cast<int8_t>(1))
			{
				blockInfo.padding = aboveBlockInfo.padding - static_cast<int8_t>(1);
				changedMesh = true;
			}

			if (northBlockInfo.blockTypes == BlockTypes::Water && northBlockInfo.padding > blockInfo.padding + static_cast<int8_t>(1))
			{
				blockInfo.padding = northBlockInfo.padding - static_cast<int8_t>(1);
				changedMesh = true;
			}

			if (eastBlockInfo.blockTypes == BlockTypes::Water && eastBlockInfo.padding > blockInfo.padding + static_cast<int8_t>(1))
			{
				blockInfo.padding = eastBlockInfo.padding - static_cast<int8_t>(1);
				changedMesh = true;
			}

			if (southBlockInfo.blockTypes == BlockTypes::Water && southBlockInfo.padding > blockInfo.padding + static_cast<int8_t>(1))
			{
				blockInfo.padding = southBlockInfo.padding - static_cast<int8_t>(1);
				changedMesh = true;
			}

			if (westBlockInfo.blockTypes == BlockTypes::Water && westBlockInfo.padding > blockInfo.padding + static_cast<int8_t>(1))
			{
				blockInfo.padding = westBlockInfo.padding - static_cast<int8_t>(1);
				changedMesh = true;
			}
		}
	}

	// Flow outwards
	if (underBlockInfo.blockTypes != BlockTypes::Air && underBlockInfo.blockTypes != BlockTypes::Water && blockInfo.padding > 1)
	{
		// North
		if (northBlockInfo.blockTypes == BlockTypes::Air)
		{
			if (northCord.z < mChunkSize.z)
			{
				northBlockInfo = BlockInfo(BlockTypes::Water, DirectionVector::North, blockInfo.padding - 1);
				changedMesh = true;
			}
			else
			{
				Vector2Int northChunkCoords = chunkData.GetChunkCoords(northCord);
				AddChunkModification(northChunkCoords, BlockInfo(BlockTypes::Water, DirectionVector::North, blockInfo.padding - 1), northCord);
			}
		}

		// East
		if (eastBlockInfo.blockTypes == BlockTypes::Air)
		{
			if (eastCord.x < mChunkSize.x)
			{
				eastBlockInfo = BlockInfo(BlockTypes::Water, DirectionVector::East, blockInfo.padding - 1);
				changedMesh = true;
			}
			else
			{
				Vector2Int eastChunkCoords = chunkData.GetChunkCoords(eastCord);
				AddChunkModification(eastChunkCoords, BlockInfo(BlockTypes::Water, DirectionVector::East, blockInfo.padding - 1), eastCord);
			}
		}

		// South
		if (southBlockInfo.blockTypes == BlockTypes::Air)
		{
			if (southCord.z >= 0)
			{
				southBlockInfo = BlockInfo(BlockTypes::Water, DirectionVector::South, blockInfo.padding - 1);
				changedMesh = true;
			}
			else
			{
				Vector2Int southChunkCoords = chunkData.GetChunkCoords(southCord);
				AddChunkModification(southChunkCoords, BlockInfo(BlockTypes::Water, DirectionVector::South, blockInfo.padding - 1), southCord);
			}
		}

		// West
		if (westBlockInfo.blockTypes == BlockTypes::Air)
		{
			if (westCord.x >= 0)
			{
				westBlockInfo = BlockInfo(BlockTypes::Water, DirectionVector::West, blockInfo.padding - 1);
				changedMesh = true;
			}
			else
			{
				Vector2Int westChunkCoords = chunkData.GetChunkCoords(westCord);
				AddChunkModification(westChunkCoords, BlockInfo(BlockTypes::Water, DirectionVector::West, blockInfo.padding - 1), westCord);
			}
		}
	}

	// Flow downwards
	if (blockIndex.y != 0 && underBlockInfo.blockTypes == BlockTypes::Air && blockInfo.padding > 0)
	{
		//const int8_t padding = Clamp(blockInfo.padding, static_cast<int8_t>(3), static_cast<int8_t>(6));
		const int8_t padding = static_cast<int8_t>(6);
		underBlockInfo = BlockInfo(BlockTypes::Water, padding);
		changedMesh = true;
	}

	// TODO: Look for a path to a source block
	// If no path -= 1
}

void WorldService::SugarCaneTick(ChunkData& chunkData, const Vector3Int& blockIndex, bool& changedMesh)
{
	// Break if there is nothing below it or not the proper block
	const BlockTypes blockUnderType = chunkData.blocksData[blockIndex.x][blockIndex.y - 1][blockIndex.z].blockTypes;
	if (blockUnderType != BlockTypes::SugarCane && blockUnderType != BlockTypes::Sand && blockUnderType != BlockTypes::GrassBlock && blockUnderType != BlockTypes::Dirt)
	{
		SoundEffectManager::Get()->Play(mLeafCrunchID, false, 1.0f, Random::UniformFloat(-0.1f, 0.15f), Random::UniformFloat(-0.1f, 0.1f));
		chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].blockTypes = BlockTypes::Air;
		changedMesh = true;
		return;
	}

	// Cant grow taller if there isnt free space above it
	if (blockIndex.y == mChunkSize.y - 1 || chunkData.blocksData[blockIndex.x][blockIndex.y + 1][blockIndex.z].blockTypes != BlockTypes::Air)
	{
		return;
	}

	// Cant grow taller then three high
	if (blockIndex.y > 1)
	{
		if (chunkData.blocksData[blockIndex.x][blockIndex.y - 1][blockIndex.z].blockTypes == BlockTypes::SugarCane
			&& chunkData.blocksData[blockIndex.x][blockIndex.y - 2][blockIndex.z].blockTypes == BlockTypes::SugarCane)
		{
			return;
		}
	}

	// Chance at growing taller
	if (Random::UniformFloat() < 0.1f)
	{
		chunkData.blocksData[blockIndex.x][blockIndex.y + 1][blockIndex.z].blockTypes = BlockTypes::SugarCane;
		changedMesh = true;
	}
}

void WorldService::OakLeavesTick(ChunkData& chunkData, const Vector3Int& blockIndex, bool& changedMesh)
{
	// Check to see if the leaf block has another leaf block beside it or a oak log

	auto CheckBlockFace = [](const BlockTypes blockType)
	{
		return blockType == BlockTypes::OakLeaves || blockType == BlockTypes::OakLog;
	};

	// Top
	if (blockIndex.y < mChunkSize.y - 1)
	{
		if (CheckBlockFace(chunkData.blocksData[blockIndex.x][blockIndex.y + 1][blockIndex.z].blockTypes))
		{
			return;
		}
	}

	// Bot
	if (blockIndex.y > 0)
	{
		if (CheckBlockFace(chunkData.blocksData[blockIndex.x][blockIndex.y - 1][blockIndex.z].blockTypes))
		{
			return;
		}
	}

	// Right
	if (blockIndex.x < mChunkSize.x - 1)
	{
		if (CheckBlockFace(chunkData.blocksData[blockIndex.x + 1][blockIndex.y][blockIndex.z].blockTypes))
		{
			return;
		}
	}

	// Left
	if (blockIndex.x > 0)
	{
		if (CheckBlockFace(chunkData.blocksData[blockIndex.x - 1][blockIndex.y][blockIndex.z].blockTypes))
		{
			return;
		}
	}

	// Front
	if (blockIndex.z < mChunkSize.z - 1)
	{
		if (CheckBlockFace(chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z + 1].blockTypes))
		{
			return;
		}
	}

	// Back
	if (blockIndex.z > 0)
	{
		if (CheckBlockFace(chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z - 1].blockTypes))
		{
			return;
		}
	}

	// Chance at disappearing
	if (Random::UniformFloat() < 0.1f)
	{
		SoundEffectManager::Get()->Play(mLeafCrunchID, false, 1.0f, Random::UniformFloat(-0.1f, 0.15f), Random::UniformFloat(-0.1f, 0.1f));
		chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].blockTypes = BlockTypes::Air;
		changedMesh = true;
	}
}

void WorldService::CactusTick(ChunkData& chunkData, const SAGE::Math::Vector3Int& blockIndex, bool& changedMesh)
{
	// Break if there is nothing below it or not the proper block or Break if there is a block beside it
	const BlockTypes blockUnderType = chunkData.blocksData[blockIndex.x][blockIndex.y - 1][blockIndex.z].blockTypes;
	if ((blockUnderType != BlockTypes::Cactus && blockUnderType != BlockTypes::Sand) ||
		(blockIndex.x < mChunkSize.x - 1 && chunkData.blocksData[blockIndex.x + 1][blockIndex.y][blockIndex.z].blockTypes != BlockTypes::Air) ||
		(blockIndex.x > 0 && chunkData.blocksData[blockIndex.x - 1][blockIndex.y][blockIndex.z].blockTypes != BlockTypes::Air) ||
		(blockIndex.z < mChunkSize.z - 1 && chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z + 1].blockTypes != BlockTypes::Air) ||
		(blockIndex.z > 0 && chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z - 1].blockTypes != BlockTypes::Air))
	{
		SoundEffectManager::Get()->Play(mLeafCrunchID, false, 1.0f, Random::UniformFloat(-0.1f, 0.15f), Random::UniformFloat(-0.1f, 0.1f));
		chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].blockTypes = BlockTypes::Air;
		changedMesh = true;
		return;
	}

	// Cant grow taller if there isnt free space above it
	if (blockIndex.y == mChunkSize.y - 1 || chunkData.blocksData[blockIndex.x][blockIndex.y + 1][blockIndex.z].blockTypes != BlockTypes::Air)
	{
		return;
	}

	// Cant grow taller then three high
	if (blockIndex.y > 1)
	{
		if (chunkData.blocksData[blockIndex.x][blockIndex.y - 1][blockIndex.z].blockTypes == BlockTypes::Cactus
			&& chunkData.blocksData[blockIndex.x][blockIndex.y - 2][blockIndex.z].blockTypes == BlockTypes::Cactus)
		{
			return;
		}
	}

	// Chance at growing taller
	if (Random::UniformFloat() < 0.1f)
	{
		chunkData.blocksData[blockIndex.x][blockIndex.y + 1][blockIndex.z].blockTypes = BlockTypes::Cactus;
		changedMesh = true;
	}
}

void WorldService::WheatTick(ChunkData& chunkData, const SAGE::Math::Vector3Int& blockIndex, bool& changedMesh)
{
	// break if there is no farm block under us
	if (chunkData.blocksData[blockIndex.x][blockIndex.y - 1][blockIndex.z].blockTypes != BlockTypes::Farmland)
	{
		chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].blockTypes = BlockTypes::Air;
		changedMesh = true;
		return;
	}

	// Chance at growing
	auto amt = chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].padding;
	if (amt < 7 && Random::UniformFloat() < 0.1f)
	{
		chunkData.blocksData[blockIndex.x][blockIndex.y][blockIndex.z].padding = Clamp(amt + 1, 0, 7);
		changedMesh = true;
	}
}

#pragma endregion