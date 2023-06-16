#pragma once

#include "Component.h"

namespace SAGE
{
	struct ParticleData
	{
		bool UpdateParticle(const float timeStep)
		{
			acceleration.y = -9.81f * gravityEffect;

			// Move Particle
			const float timeStepSqr = SAGE::Math::Sqr(timeStep);
			const auto newPosition = (position * 2.0f) - lastPosition + (acceleration * timeStepSqr);
			lastPosition = position;
			position = newPosition;

			// Update Rotation
			if (rotateOverTime) {
				currentRotation = SAGE::Math::Lerp(startRotation, endRotation, elapsedTime / lifeLength);
			}

			// Update Scale
			if (scaleOverTime) {
				currentScale = SAGE::Math::Lerp(startScale, endScale, elapsedTime / lifeLength);
			}

			// Update Color
			if (changeColorOverTime) {
				currentColor = SAGE::Math::Lerp(static_cast<SAGE::Math::Vector4>(startColor), static_cast<SAGE::Math::Vector4>(endColor), elapsedTime / lifeLength);
			}

			// Update index
			if (changeSpriteOverTime) {
				index = SAGE::Math::Lerp(0, maxIndex, elapsedTime / lifeLength);
			}

			elapsedTime += timeStep;
			return elapsedTime < lifeLength;
		}

		void ResetParticle()
		{
			currentRotation = startRotation;
			currentScale = startScale;
			currentColor = startColor;

			SetVelocity(startVelocity);

			elapsedTime = 0.0f;
		}

		void SetPosition(const SAGE::Math::Vector3& pos)
		{
			position = pos;
			lastPosition = pos;
		}

		void SetVelocity(const SAGE::Math::Vector3& velocity)
		{
			lastPosition = position - velocity;
			startVelocity = velocity;
		}

		SAGE::Math::Vector3 position = SAGE::Math::Vector3::Zero;
		SAGE::Math::Vector3 lastPosition = SAGE::Math::Vector3::Zero;
		SAGE::Math::Vector3 acceleration = SAGE::Math::Vector3::Zero;
		SAGE::Math::Vector3 startVelocity = SAGE::Math::Vector3::Zero;

		float gravityEffect = 1.0f;
		float lifeLength = 1.0f; // Cant be <= 0.0f

		bool rotateOverTime = false;
		float currentRotation = 0.0f;
		float startRotation = 0.0f;
		float endRotation = 0.0f;

		bool scaleOverTime = false;
		float currentScale = 1.0f;
		float startScale = 1.0f;
		float endScale = 1.0f;

		bool changeColorOverTime = false;
		SAGE::Graphics::Color currentColor = SAGE::Graphics::Colors::White;
		SAGE::Graphics::Color startColor = SAGE::Graphics::Colors::White;
		SAGE::Graphics::Color endColor = SAGE::Graphics::Colors::White;

		bool changeSpriteOverTime = false;
		int index = 0;
		int maxIndex = 0;

		float elapsedTime = 0.0f;
	};

	class CameraService;

	class ParticleComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Particle)
		MEMORY_POOL_DECLARE

		void Initialize() override;
		void Terminate() override;
		void DebugUI() override;

		void AddParticle(ParticleData& particle);

		void SetTextureSheet(const char* textureID, bool useTextureSheet = false, int rows = 1, int columns = 1);
		const bool UsingTextureSheet() const { return mUsingTextureSheet; }

		const SAGE::Math::Vector3 GetSpawnPoint() const { return mSpawnPoint; }
		void SetSpawnPoint(const SAGE::Math::Vector3 spawnPoint){ mSpawnPoint = spawnPoint; }

		const int GetMaxParticleCount() const { return mMaxParticles; }
		void SetMaxParticleCount(const int maxParticles) { mMaxParticles = maxParticles; }

		const bool GetIsLooping() const { return mIsLooping; }
		void SetIsLooping(const bool isLooping) {  mIsLooping = isLooping; }
		
		void SortParticles(const SAGE::Math::Vector3& cameraPos);
		SAGE::Math::Vector2 UpdateUVs(int index);

		SAGE::Graphics::RenderObject mParticleQuad;
		std::vector<ParticleData> mParticles;

	private:
		void BuildMesh(const SAGE::Math::Vector2 uv);
		int Partition(std::vector<ParticleData>& particleList, int low, int high, const SAGE::Math::Vector3 cameraPos);
		void QuickSort(std::vector<ParticleData>& particleList, int low, int high, const SAGE::Math::Vector3 cameraPos);

		SAGE::Math::Vector3 mSpawnPoint = SAGE::Math::Vector3::Zero;
		int mMaxParticles = 100;
		bool mIsLooping = false;

		bool mUsingTextureSheet = false;
		int mRows = 1;
		int mColumns = 1;
	};
}