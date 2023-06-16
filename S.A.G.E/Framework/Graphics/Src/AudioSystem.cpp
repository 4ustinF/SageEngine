#include "Precompiled.h"
#include "AudioSystem.h"

#include <DirectXTK/Inc/Audio.h>

using namespace DirectX;
using namespace SAGE::Graphics;

namespace
{
	AudioSystem* sAudioSystem = nullptr;
}

void AudioSystem::StaticInitialize()
{
	ASSERT(sAudioSystem == nullptr, "[AudioSystem] System already initialized!");
	sAudioSystem = new AudioSystem();
	sAudioSystem->Initialize();
}

void AudioSystem::StaticTerminate()
{
	if (sAudioSystem != nullptr)
	{
		sAudioSystem->Terminate();
		SafeDelete(sAudioSystem);
	}
}

AudioSystem* AudioSystem::Get()
{
	ASSERT(sAudioSystem != nullptr, "[AudioSystem] No system registered.");
	return sAudioSystem;
}

AudioSystem::AudioSystem()
	: mAudioEngine(nullptr)
{
}

AudioSystem::~AudioSystem()
{
	ASSERT(mAudioEngine == nullptr, "[AudioSystem] Terminate() must be called to clean up!");
}

void AudioSystem::Initialize()
{
	ASSERT(mAudioEngine == nullptr, "[AudioSystem] System already initialized.");

	AUDIO_ENGINE_FLAGS flags = AudioEngine_Default;
#if defined(_DEBUG)
	flags = flags | AudioEngine_Debug;
#endif

	mAudioEngine = new DirectX::AudioEngine(flags);
}

void AudioSystem::Terminate()
{
	SafeDelete(mAudioEngine);
}

void AudioSystem::Update()
{
	if (mAudioEngine && !mAudioEngine->Update())
	{
		// No audio device is active
		if (mAudioEngine->IsCriticalError())
		{
			LOG("[AudioSystem] Critical Error. Shutting down.");
			SafeDelete(mAudioEngine);
		}
	}
}