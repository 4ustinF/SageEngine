#include "Precompiled.h"
#include "SoundEffectManager.h"

#include "AudioSystem.h"
#include <DirectXTK/Inc/Audio.h>

using namespace DirectX;
using namespace SAGE::Graphics;

namespace
{
	SoundEffectManager* sSoundEffectManager = nullptr;
}

void SoundEffectManager::StaticInitialize(const char* root)
{
	ASSERT(sSoundEffectManager == nullptr, "[SoundEffectManager] Manager already initialized!");
	sSoundEffectManager = new SoundEffectManager();
	sSoundEffectManager->SetRootPath(root);
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::StaticTerminate()
{
	if (sSoundEffectManager != nullptr)
	{
		sSoundEffectManager->Clear();
		SafeDelete(sSoundEffectManager);
	}
}

//----------------------------------------------------------------------------------------------------

SoundEffectManager* SoundEffectManager::Get()
{
	ASSERT(sSoundEffectManager != nullptr, "[SoundEffectManager] No instance registered.");
	return sSoundEffectManager;
}

//----------------------------------------------------------------------------------------------------

SoundEffectManager::SoundEffectManager()
{
}

//----------------------------------------------------------------------------------------------------

SoundEffectManager::~SoundEffectManager()
{
	ASSERT(mInventory.empty(), "[SoundEffectManager] Clear() must be called to clean up.");
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::SetRootPath(const char* path)
{
	mRoot = path;
}

//----------------------------------------------------------------------------------------------------

SoundId SoundEffectManager::Load(const char* fileName)
{
	std::string fullName = mRoot + "/" + fileName;

	std::hash<std::string> hasher;
	SoundId hash = hasher(fullName);

	auto result = mInventory.insert({ hash, nullptr });
	if (result.second)
	{
		wchar_t wbuffer[1024];
		mbstowcs_s(nullptr, wbuffer, fullName.c_str(), 1024);

		auto entry = std::make_unique<Entry>();
		entry->effect = std::make_unique<SoundEffect>(AudioSystem::Get()->mAudioEngine, wbuffer);
		entry->instance = entry->effect->CreateInstance();
		result.first->second = std::move(entry);
	}

	return hash;
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::Clear()
{
	AudioSystem::Get()->mAudioEngine->Suspend();

	for (auto& item : mInventory)
	{
		if (item.second)
		{
			item.second->instance->Stop();
			item.second->instance.reset();
			item.second->effect.reset();
			item.second.reset();
		}
	}
	mInventory.clear();
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::Play(SoundId id, bool loop, float volume, float pitch, float pan)
{
	auto iter = mInventory.find(id);
	if (iter != mInventory.end())
	{
		if (loop)
		{
			iter->second->instance->Play(true);
			//iter->second->instance->SetVolume(volume);
			//iter->second->instance->SetPitch(pitch);
			//iter->second->instance->SetPan(pan);
		}
		else
		{
			iter->second->effect->Play(volume, pitch, pan);
		}
	}
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::Stop(SoundId id)
{
	auto iter = mInventory.find(id);
	if (iter != mInventory.end())
	{
		iter->second->instance->Stop(true);
	}
}