#include "Precompiled.h"
#include "ModelManager.h"

using namespace SAGE;
using namespace SAGE::Graphics;

namespace
{
	std::unique_ptr<ModelManager> sInstance;
}

void ModelManager::StaticInitialize()
{
	ASSERT(sInstance == nullptr, "ModelManager -- System already initialized!");
	sInstance = std::make_unique<ModelManager>();
}

void ModelManager::StaticTerminate()
{
	if (sInstance != nullptr)
	{
		sInstance.reset();
	}
}

ModelManager* ModelManager::Get()
{
	ASSERT(sInstance != nullptr, "ModelManager -- No system registered.");
	return sInstance.get();
}