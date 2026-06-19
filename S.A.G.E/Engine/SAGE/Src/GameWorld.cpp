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

	LoadComponentNames();

	mInitialized = true;
}

void GameWorld::Terminate()
{
	ASSERT(!mUpdating, "GameWorld - Cannot terminate world during update.");
	if (!mInitialized) { return; }

	mHierarchySections.clear();
	mComponentNames.clear();

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

	fclose(file);

	rj::Document document;
	document.ParseStream(readStream);

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

	auto gameObjects = document["GameObjects"].GetArray();
	for (auto& gameObject : gameObjects)
	{
		const char* templateFile = gameObject["TemplateFile"].GetString();
		auto newObject = CreateGameObject(templateFile);

		const char* name = gameObject["Name"].GetString();
		newObject->SetName(name);

		if (gameObject.HasMember("Components"))
		{
			for (auto& component : gameObject["Components"].GetObj())
			{
				const char* componentName = component.name.GetString();
				if (strcmp(componentName, "TransformComponent") == 0)
				{
					//auto transformComponent = gameObject.AddComponent<TransformComponent>();
					if (component.value.HasMember("Position"))
					{
						const auto& position = component.value["Position"].GetArray();
						const float x = position[0].GetFloat();
						const float y = position[1].GetFloat();
						const float z = position[2].GetFloat();
						auto transform = newObject->GetComponent<TransformComponent>();
						transform->position = {x, y, z};

						auto rigidBody = newObject->GetComponent<RigidBodyComponent>();
						if (rigidBody != nullptr)
						{
							auto rb = rigidBody->GetRigidBody();
							rb->setWorldTransform(ConvertToBtTransform(*transform));
						}
					}
				}
			}
		}
	}
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

	// Add gameobject to update list
	mUpdateList.push_back(newObject.get());

	// Dirty Hierarchy
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
	mHierarchySections.clear();

	for (auto& object : mUpdateList) 
	{
		std::string hierarchyPath = object->GetHierarchyPath();
		if (hierarchyPath.size() == 0) // Group everything into a section even if it doesn't have a section.
		{
			hierarchyPath = mRemainingSectionName;
		}

		bool bAddedToHierarchy = false;

		// Check if an section already exist.
		for (HierarchySection& section : mHierarchySections)
		{
			if (section.name == hierarchyPath)
			{
				section.hierarchyNodes.push_back(object);
				bAddedToHierarchy = true;
				break;
			}
		}

		// Create new section if there wasn't one preexisting to add to.
		if (bAddedToHierarchy == false)
		{
			HierarchySection newHierarchySection = HierarchySection(hierarchyPath);
			newHierarchySection.hierarchyNodes.push_back(object);
			mHierarchySections.push_back(newHierarchySection);
		}
	}

	mHierarchyDirty = false;
}

void GameWorld::DrawHierarchy()
{
	if (mHierarchyDirty)
	{
		RebuildHierarchy();
	}

	// List of Services
	for (auto& service : mServices) {
		const std::string objectName = service.get()->GetServiceName() + "##GameWorld";
		if (ImGui::Button(objectName.c_str()))
		{
			mInspectorService = service.get();
			mInspectorGameObject = nullptr;
			mAddComponentWindowActive = false;
		}
	}

	ImGui::Separator(); // --------------------------------------------------

	// List of sections
	for (const HierarchySection& section : mHierarchySections)
	{
		const std::string sectionName = section.name + "##GameWorld";
		if (ImGui::CollapsingHeader(sectionName.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
		{
			// List of game objects
			for (auto& object : section.hierarchyNodes)
			{
				const std::string objectName = object->GetName() + "##GameWorld";
				if (ImGui::Button(objectName.c_str()))
				{
					mInspectorService = nullptr;
					mInspectorGameObject = object;
				}
			}
		}
	}
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
	}

	if (mAddComponentWindowActive)
	{
		if (mInspectorGameObject == nullptr)
		{
			mAddComponentWindowActive = false;
		}
		else
		{
			ImGui::Begin("AddComponent##GameWorld", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
			DrawAddComponentWindow();
			ImGui::End();
		}
	}
}

void GameWorld::LoadComponentNames() // TODO:
{
	//mComponentNames.push_back("TransformComponent");
}

void GameWorld::DrawAddComponentWindow()
{
	ImGui::Text("TODO: Setup Add Comp Window"); // TODO: Remove this.

	for (const char* compName : mComponentNames)
	{
		ImGui::Text(compName);
		ImGui::SameLine();

		if (ImGui::Button("Add")) // TODO:
		{
			//GameObjectFactory::TryMakeComponent(compName, {}, *mInspectorGameObject);
		}
	}
}