#include "Precompiled.h"
#include "GameWorld.h"
#include "GameObjectFactory.h"

#include "CameraService.h"
#include "RenderService.h"
#include "TerrainService.h"

#include "MeshFilterComponent.h"
#include "ModelComponent.h"
#include "RigidBodyComponent.h"
#include "RigidBodyComponentOld.h"
#include "SelectionBoxComponent.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
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

	mInputSystem = Input::InputSystem::Get();
	mCameraService = GetService<CameraService>();

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

	mInputSystem = nullptr;
	mCameraService = nullptr;
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
		if (IsValid(gameObject->GetHandle()) && gameObject->IsActiveInHierarchy()) {
			gameObject->Update(deltaTime);
		}
	}

	// Allows game object destructions
	mUpdating = false;

	// Now we can safely destroy objects
	ProcessDestroyList();

	// ------------------------------------------------------------
	if (mEditMode)
	{
		UpdateEditSelection();
	}
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
		if (strcmp(serviceName, "Render Service") == 0)
		{
			RenderService* renderService = GetService<RenderService>();
			float skyBoxSize = -1.0f;
			if (service.value.HasMember("SkyBoxSize"))
			{
				skyBoxSize = service.value["SkyBoxTexture"].GetFloat();
			}

			if (service.value.HasMember("SkyBoxTexture"))
			{
				auto skyBoxTexture = service.value["SkyBoxTexture"].GetString();
				renderService->LoadCrossCubeMapSkyBox(skyBoxTexture, skyBoxSize);
			}
			else if (service.value.HasMember("SkyBoxTextures"))
			{
				auto skyBoxTextures = service.value["SkyBoxTextures"].GetArray();

				std::vector<const char*> fileNames;
				fileNames.reserve(6);
				fileNames.push_back(skyBoxTextures[0].GetString());
				fileNames.push_back(skyBoxTextures[1].GetString());
				fileNames.push_back(skyBoxTextures[2].GetString());
				fileNames.push_back(skyBoxTextures[3].GetString());
				fileNames.push_back(skyBoxTextures[4].GetString());
				fileNames.push_back(skyBoxTextures[5].GetString());

				renderService->LoadCubeMapSkyBox(fileNames, skyBoxSize);
			}
			else if (service.value.HasMember("SkyDomeTexture"))
			{
				auto skyDomeTexture = service.value["SkyDomeTexture"].GetString();
				renderService->LoadSkyDome(skyDomeTexture, 256, skyBoxSize); // TODO: Support divisions.
			}
		}
		else if (strcmp(serviceName, "Terrain Service") == 0)
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

	FILE* file = nullptr;
	auto err = fopen_s(&file, templateFile.u8string().c_str(), "r"); // TODO: This is a double read because of GameObjectFactory::Make(templateFile, *newObject); lets fix this
	ASSERT(err == 0 && file != nullptr, "GameWorld --- Failed to open level file '%s'", templateFile.u8string().c_str());

	char readBuffer[65536];
	rj::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));

	rj::Document document;
	document.ParseStream(readStream);

	fclose(file);

	bool isSelfActive = true;
	bool isActiveInHiearchy = false;
	if (document.HasMember("Is Active") && document["Is Active"].IsBool())
	{
		isSelfActive = document["Is Active"].GetBool();
	}

	if (isSelfActive == true)
	{
		isActiveInHiearchy = parentGO != nullptr ? parentGO->IsActiveInHierarchy() : true;
	}

	newObject->PreSeedSetActive(isSelfActive, isActiveInHiearchy);

	// Initialize game object
	newObject->Initialize();

	// Add game object to update list
	mUpdateList.push_back(newObject.get());

	// Dirty Hierarchy
	mHierarchyDirty = true;

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

const GameObject* GameWorld::GetGameObject(GameObjectHandle handle) const
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

#pragma region ---Hierarchy & Inspector---

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

	// --- Game World Settings ---
	if (ImGui::CollapsingHeader("Editor Settings##GameWorld", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Checkbox("Edit Mode##GameWorld", &mEditMode);

		ImGui::Text("Vertex Count: %s", std::to_string(mVertexCount).c_str());
		ImGui::SameLine();
		if (ImGui::Button("Update##GameWorld"))
		{
			UpdateVertexCount();
		}
	}

	ImGui::Separator();

	// --- Services ---
	for (auto& service : mServices)
	{
		const std::string name = service->GetServiceName() + "##GameWorld";

		if (ImGui::Button(name.c_str()))
		{
			if(mInspectorGameObject != nullptr && mInspectorGameObject->GetIsReparenting())
			{
				mInspectorGameObject->SetIsReparenting(false);
			}

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

	if (mRevealHierarchyPending)
	{
		mRevealHierarchyPending = false;
		mHierarchyRevealPath.clear();
	}
}

void GameWorld::DrawGameObjectNode(GameObject* object)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (object->GetChildrenHandles().empty())
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	const bool isSelected = mInspectorGameObject == object;
	if (isSelected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	const GameObjectHandle objectHandle = object->GetHandle();

	if (mRevealHierarchyPending && IsInHierarchyRevealPath(objectHandle))
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);

	const std::string label = object->GetName() + "###GameObject_" + std::to_string(reinterpret_cast<uintptr_t>(object));
	const bool open = ImGui::TreeNodeEx(label.c_str(), flags);

	if (mRevealHierarchyPending && isSelected)
	{
		ImGui::SetScrollHereY(0.5f);
	}

	// Selection
	if (ImGui::IsItemClicked())
	{
		if (mInspectorGameObject != nullptr && mInspectorGameObject->GetIsReparenting())
		{
			mInspectorGameObject->SetParent(object);
			mHierarchyDirty = true;
		}
		else
		{
			mInspectorService = nullptr;
			mInspectorGameObject = object;
			mAddComponentWindowActive = false;
		}
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
		if (!mInspectorGameObject->GetIsReparenting())
		{
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

#pragma endregion

#pragma region ---Edit Mode---

void GameWorld::UpdateEditSelection()
{
	if (!mInputSystem->IsMousePressed(Input::MouseButton::LBUTTON))
	{
		return;
	}

	if (mCameraService == nullptr)
	{
		return;
	}

	if (mInspectorGameObject != nullptr && mInspectorGameObject->GetIsReparenting())
	{
		mInspectorGameObject->SetIsReparenting(false);
	}

	const Camera& camera = mCameraService->GetCamera();
	GameObject* selectedGameObject = nullptr;
	float closestDistance = FLT_MAX;

	const auto& graphicsSystem = GraphicsSystem::Get();
	const auto screenWidth = graphicsSystem->GetBackBufferWidth();
	const auto screenHeight = graphicsSystem->GetBackBufferHeight();

	// 0. Get mouse position
	const float mouseX = Math::Max(static_cast<float>(mInputSystem->GetMouseScreenX()), Constants::Epsilon);
	const float mouseY = Math::Max(static_cast<float>(mInputSystem->GetMouseScreenY()), Constants::Epsilon);

	// 1. Convert to NDC space [-1, 1]
	float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
	float ndcY = -(2.0f * mouseY) / screenHeight + 1.0f; // flip Y (screen Y is top-down)

	// 2. Build inverse matrices
	const Matrix4 invProj = Inverse(camera.GetProjectionMatrix());
	const Matrix4 invView = Inverse(camera.GetViewMatrix());

	// 3. Unproject a point on the near plane (z=0) and far plane (z=1) in clip space
	const Vector3 nearPointNDC = Vector3(ndcX, ndcY, 0.0f);
	const Vector3 farPointNDC = Vector3(ndcX, ndcY, 1.0f);

	// Clip -> View space
	const Vector3 nearPointView = TransformCoord(nearPointNDC, invProj);
	const Vector3 farPointView = TransformCoord(farPointNDC, invProj);

	// View -> World space
	const Vector3 nearPointWorld = TransformCoord(nearPointView, invView);
	const Vector3 farPointWorld = TransformCoord(farPointView, invView);

	// 4. Direction = far - near, normalized
	const Vector3 rayDir = Normalize(farPointWorld - nearPointWorld);

	// 5. Create a ray from the camera position in the direction of the rayDir
	const Ray ray = Ray(camera.GetPosition(), rayDir);

	// TODO: Iterate over objects as via hierarchy starting parent down. So if the parent has a selection box we use that first.
	for (const GameObjectHandle& rootHandle : mRootGameObjectHandles)
	{
		if (!IsValid(rootHandle))
		{
			continue;
		}

		if (GameObject* go = GetGameObject(rootHandle))
		{
			UpdateEditSelectionRecursive(go, ray, closestDistance, selectedGameObject);
		}
	}

	if (selectedGameObject != nullptr)
	{
		mInspectorService = nullptr;
		mInspectorGameObject = selectedGameObject;
		mAddComponentWindowActive = false;

		RequestRevealInHierarchy(selectedGameObject);
	}
}

void GameWorld::UpdateEditSelectionRecursive(GameObject* gameObject, const Ray& ray, float& closestDistance, GameObject*& selectedGameObject)
{
	if (!gameObject->IsActiveInHierarchy())
	{
		return;
	}

	bool iterateOverChildren = true;

	if (SelectionBoxComponent* selectionBox = gameObject->GetComponent<SelectionBoxComponent>())
	{
		const bool isAlreadySelected = mInspectorGameObject != nullptr && gameObject == mInspectorGameObject;
		if (!isAlreadySelected)
		{
			Vector3 point;
			Vector3 normal;
			if (Intersect(ray, selectionBox->GetGlobalBoundingBox(), point, normal))
			{
				const float distance = Magnitude(point - ray.origin);
				if (distance < closestDistance)
				{
					selectedGameObject = gameObject;
					closestDistance = distance;
				}

				iterateOverChildren = false;
			}
		}
	}
	else if (const MeshFilterComponent* meshFilter = gameObject->GetComponent<MeshFilterComponent>())
	{
		if (!Intersect(ray, meshFilter->GetGlobalBoundingBox()))
		{
			return;
		}

		RayHit outHit;
		const TransformComponent* transformComponent = gameObject->GetComponent<TransformComponent>();
		const Matrix4 world = transformComponent == nullptr ? Matrix4::Identity :
			Matrix4::Scaling(transformComponent->GetScale()) *
			Matrix4::RotationQuaternion(transformComponent->GetRotation()) *
			Matrix4::Translation(transformComponent->GetPosition());
		if (IntersectRayMesh(ray, meshFilter->GetMesh(), outHit, world))
		{
			if (outHit.distance < closestDistance)
			{
				selectedGameObject = gameObject;
				closestDistance = outHit.distance;
			}
		}
	}
	//else if (const ModelComponent* modelComponent = gameObject->GetComponent<ModelComponent>()) // TODO:
	//{
	//	//if (!Intersect(ray, meshFilter->GetGlobalBoundingBox())) // TODO:
	//	//{
	//	//	return;
	//	//}

	//	RayHit outHit;
	//	const TransformComponent* transformComponent = gameObject->GetComponent<TransformComponent>();
	//	const Matrix4 world = transformComponent == nullptr ? Matrix4::Identity :
	//		Matrix4::Scaling(transformComponent->GetScale()) *
	//		Matrix4::RotationQuaternion(transformComponent->GetRotation()) *
	//		Matrix4::Translation(transformComponent->GetPosition());

	//	for (const Model::MeshData& meshData : modelComponent->GetModel().meshData)
	//	{
	//		if (IntersectRayMesh(ray, meshData.mesh, outHit, world))
	//		{
	//			if (outHit.distance < closestDistance)
	//			{
	//				selectedGameObject = gameObject;
	//				closestDistance = outHit.distance;
	//			}
	//		}
	//	}
	//}

	if (iterateOverChildren)
	{
		for (const GameObjectHandle& handle : gameObject->GetChildrenHandles())
		{
			if (GameObject* go = GetGameObject(handle))
			{
				UpdateEditSelectionRecursive(go, ray, closestDistance, selectedGameObject);
			}
		}
	}
}

void GameWorld::RequestRevealInHierarchy(GameObject* gameObject)
{
	mHierarchyRevealPath.clear();
	GameObject* current = gameObject;
	while (current != nullptr && IsValid(current->GetHandle()))
	{
		mHierarchyRevealPath.push_back(current->GetHandle());

		const GameObjectHandle parentHandle = current->GetParentHandle();
		if (!IsValid(parentHandle))
		{
			break;
		}

		current = GetGameObject(parentHandle);
	}

	mRevealHierarchyPending = true;
}

bool GameWorld::IsInHierarchyRevealPath(GameObjectHandle handle) const
{
	return std::find(
		mHierarchyRevealPath.begin(),
		mHierarchyRevealPath.end(),
		handle) != mHierarchyRevealPath.end();
}

void GameWorld::UpdateVertexCount()
{
	mVertexCount = 0;
	for (const auto& gameObject : mUpdateList)
	{
		if (IsValid(gameObject->GetHandle()) && gameObject->IsActiveInHierarchy()) 
		{
			if (const MeshFilterComponent* meshFilterComponent = gameObject->GetComponent<MeshFilterComponent>())
			{
				mVertexCount += meshFilterComponent->GetRenderObject().meshBuffer.GetVertexCount();
			}
			// TODO: Add support for other components that have vertex data, such as ModelComp,SkinnedMeshComponent, etc.
		}
	}
}

#pragma endregion