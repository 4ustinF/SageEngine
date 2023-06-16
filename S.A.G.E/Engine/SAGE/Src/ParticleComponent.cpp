#include "Precompiled.h"
#include "ParticleComponent.h"

#include "CameraService.h"
#include "ParticleService.h"
#include "GameObject.h"
#include "GameWorld.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(ParticleComponent, 400);

void ParticleComponent::Initialize()
{
	// Register to the particle service
	GetOwner().GetWorld().GetService<ParticleService>()->Register(this);

	mParticleQuad.diffuseMapId = TextureManager::Get()->LoadTexture("White.png");
	BuildMesh({ 1.0f, 1.0f });
}

void ParticleComponent::Terminate()
{
	GetOwner().GetWorld().GetService<ParticleService>()->Unregister(this);

	mParticles.clear();
	mParticleQuad.Terminate();
}

void ParticleComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Particle Component##ParticleComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat3("Spawn Point##ParticleComponent", &mSpawnPoint.x, 0.1f);
		ImGui::Checkbox("Loop Particles##ParticleComponent", &mIsLooping);
		ImGui::InputInt("Max Number Of Particles##ParticleComponent", &mMaxParticles, 1);
		int numOfParticles = static_cast<int>(mParticles.size());
		ImGui::InputInt("Number Of Particles##ParticleComponent", &numOfParticles, 0);
	}
}

void ParticleComponent::AddParticle(ParticleData& particle)
{
	if (mParticles.size() >= mMaxParticles) { return; }

	if (mUsingTextureSheet)
	{
		particle.maxIndex = (mRows * mColumns) - 1;
	}
	particle.changeSpriteOverTime = mUsingTextureSheet;

	mParticles.push_back(std::move(particle));
}

void ParticleComponent::SetTextureSheet(const char* textureID, bool useTextureSheet, int rows, int columns)
{
	mParticleQuad.diffuseMapId = TextureManager::Get()->LoadTexture(textureID);
	mUsingTextureSheet = useTextureSheet;
	mRows = rows;
	mColumns = columns;
	if (mRows <= 0) { mRows = 1; }
	if (mColumns <= 0) { mColumns = 1; }

	// Rebuild Mesh With new UVs
	if (mUsingTextureSheet) {
		BuildMesh({ 1.0f / mColumns, 1.0f / mRows });
	}
}

void ParticleComponent::SortParticles(const SAGE::Math::Vector3& cameraPos)
{
	QuickSort(mParticles, 0, static_cast<int>(mParticles.size()) - 1, cameraPos);
}

Vector2 ParticleComponent::UpdateUVs(int index)
{
	SAGE::Math::Clamp(index, 0, mRows * mColumns - 1);

	// Get x and y pos 
	const int x = index % mColumns;
	const int y = index / mColumns;

	return SAGE::Math::Vector2((1.0f / mColumns) * x, (1.0f / mRows) * y);
}

void ParticleComponent::BuildMesh(const Vector2 uv)
{
	OldMesh quadMesh;
	const float size = 0.25f;
	quadMesh.vertices.push_back({ {-size, +size, 0.0f}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, 0.0f} }); // 0 Top Left
	quadMesh.vertices.push_back({ {+size, +size, 0.0f}, -Vector3::ZAxis, Vector3::XAxis, {uv.x, 0.0f} }); // 1 Top Right
	quadMesh.vertices.push_back({ {-size, -size, 0.0f}, -Vector3::ZAxis, Vector3::XAxis, {0.0f, uv.y} }); // 2 Bot Left
	quadMesh.vertices.push_back({ {+size, -size, 0.0f}, -Vector3::ZAxis, Vector3::XAxis, {uv.x, uv.y} }); // 3 Bot Right
	quadMesh.indices = { 0, 1, 2, 3, 2, 1 };
	mParticleQuad.meshBuffer.Initialize(quadMesh);
}

int ParticleComponent::Partition(std::vector<ParticleData>& particleList, int low, int high, const SAGE::Math::Vector3 cameraPos)
{
	// Select the rightmost element as pivot
	float pivot = Distance(particleList[high].position, cameraPos);

	// Pointer for greater element
	int i = (low - 1);

	// Traverse each element of the array
	// Compare them with the pivot
	for (int j = low; j < high; j++) {
		if (Distance(particleList[j].position, cameraPos) > pivot) {
			// If element smaller than pivot is found
			// Swap it with the greater element pointed by i
			i++;

			// Swap element at i with element at j
			std::swap(particleList[i], particleList[j]);
		}
	}

	// Swap pivot with the greater element at i
	std::swap(particleList[i + 1], particleList[high]);

	// Return the partition point
	return (i + 1);
}

void ParticleComponent::QuickSort(std::vector<ParticleData>& particleList, int low, int high, const SAGE::Math::Vector3 cameraPos)
{
	if (low < high) {

		// Find the pivot element such that
		// Elements smaller than pivot are on left of pivot
		// Elements greater than pivot are on righ of pivot
		int pi = Partition(particleList, low, high, cameraPos);

		// Recursive call on the left of pivot
		QuickSort(particleList, low, pi - 1, cameraPos);

		// Recursive call on the right of pivot
		QuickSort(particleList, pi + 1, high, cameraPos);
	}
}