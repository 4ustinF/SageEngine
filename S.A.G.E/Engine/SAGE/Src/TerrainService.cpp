#include "Precompiled.h"
#include "TerrainService.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void TerrainService::Initialize()
{
	SetServiceName("Terrain Service");
	//mTerrain.Initialize("../../Assets/Heightmaps/Heightmap.raw", 5.0f);

	//auto tm = TextureManager::Get();
	//mTerrainRenderObject.diffuseMapId = tm->LoadTexture("grass.jpg");
	//mTerrainRenderObject.specularMapId = tm->LoadTexture("dirt.jpg");

	//mTerrainRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	//mTerrainRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	//mTerrainRenderObject.material.specular = { 0.1f, 0.1f, 0.1f, 1.0f };
	//mTerrainRenderObject.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	//mTerrainRenderObject.material.power = 10.0f;

	//mTerrainRenderObject.meshBuffer.Initialize(mTerrain.mesh);
}

void TerrainService::Terminate() 
{
	mTerrainRenderObject.Terminate();
}

void TerrainService::DebugUI()
{
	ImGui::Begin("Terrain Service", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::CollapsingHeader("Material##Terrain", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Terrain", &mTerrainRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Terrain", &mTerrainRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Terrain", &mTerrainRenderObject.material.specular.r);
		ImGui::ColorEdit4("Emissive##Terrain", &mTerrainRenderObject.material.emissive.r);
		ImGui::DragFloat("Power##Terrain", &mTerrainRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	}

	ImGui::End();
}

void TerrainService::LoadTerrain(const char* fileName, float maxHeight)
{
	mTerrain.Initialize(fileName, maxHeight);

	mTerrainRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mTerrainRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mTerrainRenderObject.material.specular = { 0.1f, 0.1f, 0.1f, 1.0f };
	mTerrainRenderObject.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	mTerrainRenderObject.material.power = 10.0f;

	mTerrainRenderObject.meshBuffer.Initialize(mTerrain.mesh);
}

void TerrainService::LoadTexture(const char* fileName, uint32_t index)
{
	auto tm = TextureManager::Get();
	if (index == 0) {
		mTerrainRenderObject.diffuseMapId = tm->LoadTexture(fileName);
	}
	else {
		mTerrainRenderObject.specularMapId = tm->LoadTexture(fileName);
	}

}

float SAGE::TerrainService::GetHeight(const Math::Vector3& position) const
{
	const int x = static_cast<int>(position.x);
	const int z = static_cast<int>(position.z);

	if (x < 0 || z < 0 || x >= mTerrain.mColumns || z >= mTerrain.mRows) {
		return 0.0f;
	}

	const uint32_t bl = static_cast<uint32_t>((x + 0) + (z + 0) * mTerrain.mColumns);
	const uint32_t tl = static_cast<uint32_t>((x + 0) + (z + 1) * mTerrain.mColumns);
	const uint32_t br = static_cast<uint32_t>((x + 1) + (z + 0) * mTerrain.mColumns);
	const uint32_t tr = static_cast<uint32_t>((x + 1) + (z + 1) * mTerrain.mColumns);

	const float u = position.x - x;
	const float v = position.z - z;

	float height = 0;
	if (u > v) // Botom right triangle
	{
		const auto& a = mTerrain.mesh.vertices[br];
		const auto& b = mTerrain.mesh.vertices[tr];
		const auto& c = mTerrain.mesh.vertices[bl];
		const auto deltaAB = b.position.y - a.position.y;
		const auto deltaAC = c.position.y - a.position.y;
		height = a.position.y + (deltaAB * (1-u)) + (deltaAC * v);
	}
	else // Top Left Triangle
	{
		const auto& a = mTerrain.mesh.vertices[tl];
		const auto& b = mTerrain.mesh.vertices[tr];
		const auto& c = mTerrain.mesh.vertices[bl];
		const auto deltaAB = b.position.y - a.position.y;
		const auto deltaAC = c.position.y - a.position.y;
		height = a.position.y + (deltaAB* u) + (deltaAC * (1-v));
	}

	return height;
}
