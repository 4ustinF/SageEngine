#pragma once

#include "Service.h"

namespace SAGE
{
	class CameraService;
	class ParticleComponent;

	class ParticleService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::Particles)

		void Initialize() override;
		void Terminate() override;

		void Update(float deltaTime) override;

		void Render() override;
		void DebugUI() override;

		void SetTimeSet(const float timeStep) { mTimeStep = SAGE::Math::Abs(timeStep); }

	private:
		const CameraService* mCameraService = nullptr;

		SAGE::Graphics::ParticleEffect mParticleEffect;

		float mTimer = 0.0f;
		float mTimeStep = 1.0f / 60.0f;

		// ----------
		friend class ParticleComponent;
		void Register(ParticleComponent* particleComponent);
		void Unregister(const ParticleComponent* particleComponent);

		std::vector<ParticleComponent*> mParticleEntries;
	};
}