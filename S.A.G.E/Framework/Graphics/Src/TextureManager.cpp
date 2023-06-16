#include "Precompiled.h"
#include "TextureManager.h"

using namespace SAGE;
using namespace SAGE::Graphics;

namespace
{
	std::unique_ptr<TextureManager> sInstance;
}

void TextureManager::StaticInitialize(std::filesystem::path root)
{
	ASSERT(sInstance == nullptr, "TextureManager -- System already is in use");
	sInstance = std::make_unique<TextureManager>();
	sInstance->SetRootDirectory(std::move(root));
}

void TextureManager::StaticTerminate()
{
	sInstance.reset();
}

TextureManager* TextureManager::Get()
{
	ASSERT(sInstance != nullptr, "TextureManager -- No System Registered");
	return sInstance.get();
}

TextureManager::~TextureManager()
{
	for (auto& [id, texture] : mInventory) {
		texture->Terminate();
	}
	mInventory.clear();
}

TextureId TextureManager::LoadTexture(std::filesystem::path fileName)
{
	auto textureId = std::filesystem::hash_value(fileName);
	auto [iter, success] = mInventory.insert({ textureId , nullptr});
	if (success) {
		auto& texturePtr = iter->second;
		texturePtr = std::make_unique<Texture>();
		texturePtr->Initialize(mRootDiretory / fileName);
	}
	return textureId;
}

void TextureManager::BindVS(TextureId id, uint32_t slot) const
{
	auto iter = mInventory.find(id);
	if (iter != mInventory.end()) {
		iter->second->BindVS(slot);
	}
}

void TextureManager::BindPS(TextureId id, uint32_t slot) const
{
	auto iter = mInventory.find(id);
	if (iter != mInventory.end()) {
		iter->second->BindPS(slot);
	}
}

Texture* TextureManager::GetTexture(TextureId id)
{
	auto iter = mInventory.find(id);
	return iter != mInventory.end() ? iter->second.get() : nullptr;
}