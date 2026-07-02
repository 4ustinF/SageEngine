#include "Precompiled.h"
#include "GameWorld.h"
#include "GameObjectFactory.h"

#include "RenderService.h"
#include "TerrainService.h"

#include "TransformComponent.h"
#include "RigidBodyComponent.h"

using namespace SAGE;
namespace rj = rapidjson;

void GameWorld::Initialize(uint32_t capacity)
{
	ASSERT(!mInitialized, "GameWorld - Already initialized.");

	for (auto& service : mServices) {
		service->Initialize();
	}

	mGameObjectSlots.resize(capacity);
	mFreeSlots.resize(capacity);
	std::iota(mFreeSlots.rbegin(), mFreeSlots.rend(), 0);

	mInitialized = true;
}

void GameWorld::Terminate()
{
	ASSERT(!mUpdating, "GameWorld - Cannot terminate world during update.");
	if (!mInitialized) { return; }

	mRootGameObjectHandles.clear();

	// Destroy all remaining game objects
	for (auto gameObject : mUpdateList) {
		DestroyGameObject(gameObject->GetHandle());
	}
	ProcessDestroyList();
	ASSERT(mUpdateList.empty(), "GameWorld - failed to clean up game objects.");

	for (std::vector<std::unique_ptr<SAGE::Service>>::reverse_iterator it = mServices.rbegin(); it != mServices.rend(); ++it) {
		it->get()->Terminate();
	}

	mInitialized = false;

	mInspectorGameObject = nullptr;
	mInspectorService = nullptr;
}

void GameWorld::Update(float deltaTime)
{
	ASSERT(!mUpdating, "GameWorld - already updating the world");

	// This defers game object destruction
	mUpdating = true;

	for (auto& service : mServices) {
		service->Update(deltaTime);
	}

	for (size_t i = 0; i < mUpdateList.size(); ++i) {
		GameObject* gameObject = mUpdateList[i];
		if (IsValid(gameObject->GetHandle())) {
			gameObject->Update(deltaTime);
		}
	}

	// Allows game object destructions
	mUpdating = false;

	// Now we can safely destroy objects
	ProcessDestroyList();
}

void GameWorld::Render()
{
	for (auto& service : mServices) {
		service->Render();
	}
}

void GameWorld::DebugUI()
{
	ImGui::Begin("Hierarchy##GameWorld", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	DrawHierarchy();
	ImGui::End();

	ImGui::Begin("Inspector##GameWorld", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	DrawInspector();
	ImGui::End();
}

void GameWorld::LoadLevel(std::filesystem::path levelFile)
{
	FILE* file = nullptr;
	auto err = fopen_s(&file, levelFile.u8string().c_str(), "r");
	ASSERT(err == 0 && file != nullptr, "GameWorld --- Failed to open level file '%s'", levelFile.u8string().c_str());

	char readBuffer[65536];
	rj::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));

	rj::Document document;
	document.ParseStream(readStream);

	fclose(file);

	auto services = document["Services"].GetObj();
	for (auto& service : services)
	{
		const char* serviceName = service.name.GetString();
		if (strcmp(serviceName, "RenderService") == 0)
		{
			if (service.value.HasMember("SkyDomeTexture"))
			{
				auto renderService = GetService<RenderService>();
				auto skyDomeTexture = service.value["SkyDomeTexture"].GetString();
				renderService->LoadSkyDome(skyDomeTexture);
			}
			if (service.value.HasMember("SkyBoxTexture"))
			{
				auto renderService = GetService<RenderService>();
				auto skyBoxTexture = service.value["SkyBoxTexture"].GetString();
				renderService->LoadSkyBox(skyBoxTexture);
			}
		}
		else if (strcmp(serviceName, "TerrainService") == 0)
		{
			auto terrainService = GetService<TerrainService>();

			const char* heightMap = nullptr;
			float maxHeight = 0.0f;
			if (service.value.HasMember("HeightMap")) {
				heightMap = service.value["HeightMap"].GetString();
			}
			if (service.value.HasMember("MaxHeight")) {
				maxHeight = service.value["MaxHeight"].GetFloat();
			}
			terrainService->LoadTerrain(heightMap, maxHeight);

			if (service.value.HasMember("Textures"))
			{
				const auto& textureArray = service.value["Textures"].GetArray();
				for (uint32_t i = 0; i < textureArray.Size(); ++i)
				{
					terrainService->LoadTexture(textureArray[i].GetString(), i);
				}
			}
		}
		// ... more services here
	}

	for (auto& gameObject : document["GameObjects"].GetArray())
	{
		if (!gameObject.HasMember("TemplateFile") || !gameObject["TemplateFile"].IsString())
		{
			continue;
		}

		const char* objectName = nullptr;
		if (gameObject.HasMember("Name") && gameObject["Name"].IsString())
		{
			objectName = gameObject["Name"].GetString();
		}

		CreateGameObjectRecursive(gameObject["TemplateFile"].GetString(), nullptr, objectName);
	}
}

GameObject* GameWorld::CreateGameObjectRecursive(std::filesystem::path templateFile, GameObject* parentGO, const char* overrideName)
{
	ASSERT(mInitialized, "GameWorld - World must be initialized frist before creating game objects.");

	if (mFreeSlots.empty()) {
		return nullptr;
	}

	// Reserve slot
	const uint32_t freeSlot = mFreeSlots.back();
	mFreeSlots.pop_back();

	auto& slot = mGameObjectSlots[freeSlot];
	auto& newObject = slot.gameObject;
	newObject = std::make_unique<GameObject>();

	// Initialize handle
	GameObjectHandle handle;
	handle.mIndex = freeSlot;
	handle.mGeneration = slot.generation;

	// Set template path
	newObject->SetTemplatePath(templateFile);

	// Set object name
	if (overrideName != nullptr)
	{
		newObject->SetName(overrideName);
	}

	// Initialize game object
	newObject->mWorld = this;
	newObject->mHandle = handle;

	// Parent/child setup
	if (parentGO != nullptr)
	{
		newObject->SetParent(parentGO);
	}

	// Attach components
	GameObjectFactory::Make(templateFile, *newObject);

	// Initialize game object
	newObject->Initialize();

	// Add game object to update list
	mUpdateList.push_back(newObject.get());

	// Dirty Hierarchy
	mHierarchyDirty = true;

	FILE* file = nullptr;
	auto err = fopen_s(&file, templateFile.u8string().c_str(), "r"); // TODO: This is a double read because of GameObjectFactory::Make(templateFile, *newObject); lets fix this
	ASSERT(err == 0 && file != nullptr, "GameWorld --- Failed to open level file '%s'", templateFile.u8string().c_str());

	char readBuffer[65536];

	rj::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));

	rj::Document document;
	document.ParseStream(readStream);

	fclose(file);

	if (document.HasMember("GameObjects") && document["GameObjects"].IsArray())
	{
		for (auto& childObject : document["GameObjects"].GetArray())
		{
			const char* childName = nullptr;
			if (childObject.HasMember("Name") && childObject["Name"].IsString())
			{
				childName = childObject["Name"].GetString();
			}

			if (childObject.HasMember("TemplateFile") && childObject["TemplateFile"].IsString())
			{
				CreateGameObjectRecursive(childObject["TemplateFile"].GetString(), newObject.get(), childName);
			}
		}
	}

	return newObject.get();
}

GameObject* GameWorld::CreateGameObject(std::filesystem::path templateFile)
{
	ASSERT(mInitialized, "GameWorld - World must be initialized frist before creating game objects.");

	if (mFreeSlots.empty()) {
		return nullptr;
	}

	const uint32_t freeSlot = mFreeSlots.back();
	mFreeSlots.pop_back();

	auto& slot = mGameObjectSlots[freeSlot];
	auto& newObject = slot.gameObject;
	newObject = std::make_unique<GameObject>();

	// Attach components
	GameObjectFactory::Make(templateFile, *newObject);

	// Initialize handle
	GameObjectHandle handle;
	handle.mIndex = freeSlot;
	handle.mGeneration = slot.generation;

	// Initialize game object
	newObject->mWorld = this;
	newObject->mHandle = handle;
	newObject->Initialize();

	// Add game object to update list
	mUpdateList.push_back(newObject.get());

	// Dirty Hierarchy
	newObject->SetTemplatePath(templateFile);
	mHierarchyDirty = true;

	return newObject.get();
}

GameObject* GameWorld::FindGameObject(const std::string& name)
{
	auto iter = std::find_if(mUpdateList.begin(), mUpdateList.end(), [&](auto gameObject) {
			return gameObject->GetName() == name;
		});
	return iter == mUpdateList.end() ? nullptr : *iter;
}

GameObject* GameWorld::GetGameObject(GameObjectHandle handle)
{
	if (IsValid(handle)) {
		return mGameObjectSlots[handle.mIndex].gameObject.get();
	}
	return nullptr;
}

void GameWorld::DestroyGameObject(GameObjectHandle handle)
{
	if (!IsValid(handle)) { return; }

	auto& slot = mGameObjectSlots[handle.mIndex];
	slot.generation++; // This invalidates all existing handles to the slot
	mToBeDestroyed.push_back(handle.mIndex);
	mHierarchyDirty = true;
}

bool GameWorld::IsValid(GameObjectHandle handle) const
{
	return
		handle.mIndex >= 0 &&
		handle.mIndex < mGameObjectSlots.size() &&
		mGameObjectSlots[handle.mIndex].generation == handle.mGeneration;
}

void GameWorld::ProcessDestroyList()
{
	ASSERT(mInitialized, "GameWorld - world must be initialized first before destroying game objects.");

	if (mToBeDestroyed.size() > 0)
	{
		mHierarchyDirty = true;
	}

	for (auto index : mToBeDestroyed)
	{
		auto& slot = mGameObjectSlots[index];

		GameObject* gameObject = slot.gameObject.get();
		ASSERT(!IsValid(gameObject->GetHandle()), "GameWorld - Game object is still valid.");

		// Remove from update list
		mUpdateList.erase(std::remove(mUpdateList.begin(), mUpdateList.end(), gameObject)); // Erase-Remove Idiom

		// Terminate gameOnject
		gameObject->Terminate();

		// Delete the game object and recycle the slot
		slot.gameObject.reset();
		mFreeSlots.push_back(index);
	}

	mToBeDestroyed.clear();
}

void GameWorld::RebuildHierarchy()
{
	mRootGameObjectHandles.clear();
	for (auto& obj : mUpdateList)
	{
		GameObjectHandle handle = obj->GetHandle();
		if (!IsValid(handle) || IsValid(obj->GetParentHandle()))
		{
			continue;
		}

		mRootGameObjectHandles.push_back(handle);
	}

	mHierarchyDirty = false;
}

void GameWorld::DrawHierarchy()
{
	if (mHierarchyDirty)
	{
		RebuildHierarchy();
	}

	// --- Services ---
	for (auto& service : mServices)
	{
		const std::string name = service->GetServiceName() + "##GameWorld";

		if (ImGui::Button(name.c_str()))
		{
			mInspectorService = service.get();
			mInspectorGameObject = nullptr;
			mAddComponentWindowActive = false;
		}
	}

	ImGui::Separator();

	// --- GameObject Hierarchy ---
	for (const GameObjectHandle& rootHandle : mRootGameObjectHandles)
	{
		if (IsValid(rootHandle))
		{
			DrawGameObjectNode(GetGameObject(rootHandle));
		}
	}
}

void GameWorld::DrawGameObjectNode(GameObject* object) // TODO: Fix indent spacing on grandchildren
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (object->GetChildrenHandles().empty())
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (mInspectorGameObject == object)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
	const std::string label = object->GetName() + "##GameWorld";
	const bool open = ImGui::TreeNodeEx(label.c_str(), flags);

	// Selection
	if (ImGui::IsItemClicked())
	{
		mInspectorService = nullptr;
		mInspectorGameObject = object;
		mAddComponentWindowActive = false;
	}

	if (open)
	{
		for (auto& childObjectHandle : object->GetChildrenHandles())
		{
			if (GameObject* childObject = GetGameObject(childObjectHandle))
			{
				DrawGameObjectNode(childObject);
			}
		}

		ImGui::TreePop();
	}

	ImGui::PopStyleVar();
}

void GameWorld::DrawInspector()
{
	if (mInspectorService != nullptr) 
	{
		mInspectorService->DebugUI(); 
		return;
	}

	if (mInspectorGameObject != nullptr) 
	{ 
		mInspectorGameObject->DebugUI();
		ImGui::Separator();

		if (ImGui::Button("Add Component"))
		{
			mAddComponentWindowActive = !mAddComponentWindowActive;
		}

		ImGui::SameLine(0.0f, mImguiSpacing);

		if (ImGui::Button("Save Game Object"))
		{
			mInspectorGameObject->SaveComponents();
		}
	}

	if (mAddComponentWindowActive)
	{
		if (mInspectorGameObject == nullptr)
		{
			mAddComponentWindowActive = false;
		}
		else
		{
			ImGui::Begin("Add Component##GameWorld", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
			DrawAddComponentWindow(); // TODO: Maybe lets not make this an extra window? Just attach it to the inspector? or at least calculate the bottom of the inspector so it stays attached.
			ImGui::End();
		}
	}
}

void GameWorld::DrawAddComponentWindow()
{
	ImGui::Text("TODO: Setup Add Comp Window"); // TODO: Remove this.
}