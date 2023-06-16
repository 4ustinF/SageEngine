#include "ParticleSystem.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void ParticleSystem::Initialize()
{
	mParticleQuad.diffuseMapId = TextureManager::Get()->LoadTexture("White.png");
	BuildMesh({ 1.0f, 1.0f });
}

void ParticleSystem::Terminate()
{
	mParticleQuad.Terminate();
	mParticles.clear();
}

void ParticleSystem::Update(float deltaTime)
{
	mTimer += deltaTime;
	while (mTimer >= mTimeStep)
	{
		mTimer -= mTimeStep;

		for (int i = 0; i < mParticles.size(); ++i)
		{
			const bool isDead = !mParticles[i].UpdateParticle(mTimeStep);
			if (isDead)
			{
				if (mIsLooping) {
					mParticles[i].position = mSpawnPoint;
					mParticles[i].ResetParticle();
				}
				else {
					std::swap(mParticles[i], mParticles.back());
					mParticles.pop_back();
				}
			}
		}
	}

	auto inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyDown(KeyCode::SPACE))
	{
		ParticleInfo particle;
		particle.SetPosition(mSpawnPoint);
		particle.SetVelocity(Random::OnUnitSphere() * 0.1f);
		particle.lifeLength = 2.0f;
		particle.gravityEffect = 0.0f;

		//particle.scaleOverTime = true;
		//particle.endScale = 0.0f;

		particle.scaleOverTime = true;
		particle.startScale = 0.0f;
		particle.currentScale = particle.startScale;
		particle.endScale = 3.0f;

		//particle.changeColorOverTime = true;
		//particle.endColor = Colors::Green;
		//particle.currentColor = Colors::Purple;
		//particle.currentColor = { Random::UniformFloat(0.0f, 1.0f), Random::UniformFloat(0.0f, 1.0f) , Random::UniformFloat(0.0f, 1.0f) , 1.0f };

		particle.rotateOverTime = true;
		particle.startRotation = 0.0f;
		//particle.endRotation = -360.0f;

		particle.endRotation = Random::UniformFloat(-180, 180);

		AddParticle(particle);
	}

	const float moveSpeed = 2.0f;
	if (inputSystem->IsKeyDown(KeyCode::RIGHT))
	{
		mSpawnPoint.x += moveSpeed * deltaTime;
	}
	else if (inputSystem->IsKeyDown(KeyCode::LEFT))
	{
		mSpawnPoint.x -= moveSpeed * deltaTime;
	}

	if (inputSystem->IsKeyDown(KeyCode::UP))
	{
		mSpawnPoint.z += moveSpeed * deltaTime;
	}
	else if (inputSystem->IsKeyDown(KeyCode::DOWN))
	{
		mSpawnPoint.z -= moveSpeed * deltaTime;
	}
}

void ParticleSystem::DebugUI()
{
	int amt = mParticles.size();
	ImGui::SliderInt("Particle Amount", &amt, 1, 0);
}

void ParticleSystem::AddParticle(ParticleInfo& particle)
{
	if (mParticles.size() >= mMaxParticles) { return; }

	if (mUsingTextureSheet)
	{
		particle.maxIndex = (mRows * mColumns) - 1;
	}
	particle.changeSpriteOverTime = mUsingTextureSheet;

	mParticles.push_back(std::move(particle));
}

void ParticleSystem::SetTextureSheet(const char* textureID, bool useTextureSheet, int rows, int columns)
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

void ParticleSystem::BuildMesh(const SAGE::Math::Vector2 uv)
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

Vector2 ParticleSystem::UpdateUVs(int index)
{
	SAGE::Math::Clamp(index, 0, mRows * mColumns - 1);

	// Get x and y pos 
	const int x = index % mColumns;
	const int y = index / mColumns;

	return SAGE::Math::Vector2((1.0f / mColumns) * x, (1.0f / mRows) * y);
}

void ParticleSystem::BubbleSort()
{
	// Sort Particles to render from back to front (Currently Using bubble sort)
	// Instance rendering can help replace this / moving to quick sort would help ease the load
	const int size = mParticles.size() - 1;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size - i; j++)
		{
			if (Distance(mParticles[j].position, CameraPos) < Distance(mParticles[j + 1].position, CameraPos))
			{
				std::swap(mParticles[j], mParticles[j + 1]);
			}
		}
	}
}

int ParticleSystem::Partition(std::vector<ParticleInfo>& particleList, int low, int high, const Vector3 cameraPos)
{
	// select the rightmost element as pivot
	//int pivot = particleList[high];
	float pivot = Distance(particleList[high].position, cameraPos);

	// pointer for greater element
	int i = (low - 1);

	// traverse each element of the array
	// compare them with the pivot
	for (int j = low; j < high; j++) {
		//if (particleList[j] <= pivot) {
		//if (Distance(particleList[j].position, cameraPos) <= pivot) {
		if (Distance(particleList[j].position, cameraPos) > pivot) {
			// if element smaller than pivot is found
			// swap it with the greater element pointed by i
			i++;

			// swap element at i with element at j
			std::swap(particleList[i], particleList[j]);
		}
	}

	// swap pivot with the greater element at i
	std::swap(particleList[i + 1], particleList[high]);

	// return the partition point
	return (i + 1);
	//return 0;
}

// https://www.programiz.com/dsa/quick-sort
void ParticleSystem::QuickSort(std::vector<ParticleInfo>& particleList, int low, int high, const Vector3 cameraPos)
{
	if (low < high) {

		// find the pivot element such that
		// elements smaller than pivot are on left of pivot
		// elements greater than pivot are on righ of pivot
		int pi = Partition(particleList, low, high, cameraPos);

		// recursive call on the left of pivot
		QuickSort(particleList, low, pi - 1, cameraPos);

		// recursive call on the right of pivot
		QuickSort(particleList, pi + 1, high, cameraPos);
	}
}