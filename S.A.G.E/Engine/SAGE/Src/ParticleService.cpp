#include "Precompiled.h"
#include "ParticleService.h"

#include "CameraService.h"
#include "ParticleComponent.h"

//#include "GameObject.h"
#include "GameWorld.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;

void ParticleService::Initialize()
{
	SetServiceName("Particle Service");
	mCameraService = GetWorld().GetService<CameraService>();

	mParticleEffect.SetBlendState(BlendState::Mode::AlphaPremultiplied);
	mParticleEffect.Initialize();
}

void ParticleService::Terminate()
{
	mParticleEffect.Terminate();
	mParticleEntries.clear();
}

void ParticleService::Update(float deltaTime)
{
	mTimer += deltaTime;
	while (mTimer >= mTimeStep)
	{
		mTimer -= mTimeStep;

		for (auto& particleComponent : mParticleEntries) // Each Particle Component in the game
		{
			for (int i = 0; i < particleComponent->mParticles.size(); ++i)
			{
				const bool isDead = !particleComponent->mParticles[i].UpdateParticle(mTimeStep);
				if (isDead)
				{
					if (particleComponent->GetIsLooping()) {
						particleComponent->mParticles[i].position = particleComponent->GetSpawnPoint();
						particleComponent->mParticles[i].ResetParticle();
					}
					else {
						std::swap(particleComponent->mParticles[i], particleComponent->mParticles.back());
						particleComponent->mParticles.pop_back();
					}
				}
			}
		}
	}
}

void ParticleService::Render()
{
	const Camera& camera = mCameraService->GetCamera();
	mParticleEffect.SetCamera(camera);
	const Vector3& cameraPos = camera.GetPosition();
	const Vector3 cameraNegPos = -cameraPos;

	mParticleEffect.Begin();
	{
		for (const auto& particleComponent : mParticleEntries) // Each Particle Component in the game
		{
			particleComponent->SortParticles(cameraPos);
			for (auto& p : particleComponent->mParticles) // Each particle in the Particle Component
			{
				auto& quad = particleComponent->mParticleQuad;
				quad.transform.position = p.position;
				quad.transform.rotation = Quaternion::RotationLook(cameraNegPos) * Quaternion::RotationAxis(Vector3::ZAxis, Constants::DegToRad * p.currentRotation);
				quad.transform.scale = Vector3(p.currentScale);
				mParticleEffect.SetParticleColor(p.currentColor);

				if (particleComponent->UsingTextureSheet())
				{
					mParticleEffect.SetUVOffSet(particleComponent->UpdateUVs(p.index));
				}

				mParticleEffect.Render(particleComponent->mParticleQuad);
			}
			particleComponent->mParticleQuad.transform.rotation = Quaternion::Identity;
		}
	}
	mParticleEffect.End();
}

void ParticleService::DebugUI()
{
	mParticleEffect.DebugUI();
}

void ParticleService::Register(ParticleComponent* particleComponent)
{
	mParticleEntries.push_back(particleComponent);
}

void ParticleService::Unregister(const ParticleComponent* particleComponent)
{
	auto iter = std::find(mParticleEntries.begin(), mParticleEntries.end(), particleComponent);
	if (iter != mParticleEntries.end())
	{
		mParticleEntries.erase(iter);
	}
}
