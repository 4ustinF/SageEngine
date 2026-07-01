#include "Precompiled.h"
#include "GameObject.h"

#include "GameWorld.h"

using namespace SAGE;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(GameObject, 1000);

void GameObject::Initialize()
{
	ASSERT(!mInitialize, "GameObject - Already initialized.");
	for (auto& component : mComponents) {
		component->Initialize();
		component->OnEnable();
	}
	mInitialize = true;
}

void GameObject::Terminate()
{
	mChildGameObjectHandles.clear(); // TODO: More clean up like removing itself from parent and children.

	for (auto& component : mComponents) {
		component->OnDisable();
		component->Terminate();
	}
}

void GameObject::Update(float deltaTime)
{
	if (!mIsActive)
	{
		return;
	}

	for (auto& component : mComponents)
	{
		component->Update(deltaTime);

		if (component->CanQueueUpdate())
		{
			component->ClearQueueUpdate();
			component->OnQueueUpdate(deltaTime);
		}
	}
}

void GameObject::DebugUI()
{
	if (ImGui::Checkbox("IsActive", &mIsActive))
	{
		SetActive(mIsActive);
	}

	for (auto& component : mComponents) {
		component->DebugUI();
	}
}

void GameObject::SetActive(bool active)
{
	mIsActive = active;
	mIsActive ? OnEnable() : OnDisable();
}

void GameObject::OnEnable()
{
	for (auto& component : mComponents) {
		component->OnEnable();
	}
}

void GameObject::OnDisable()
{
	for (auto& component : mComponents) {
		component->OnDisable();
	}
}

void GameObject::SaveComponents()
{
	if (mTemplatePath.empty())
	{
		return; // Log that we cannot save this object to disk currently.
	}

	FILE* file = nullptr;
	errno_t err =
		fopen_s(&file, mTemplatePath.u8string().c_str(), "r");

	assert(err == 0);
	assert(file != nullptr);

	//FILE* file = nullptr;
	//fopen_s(&file, mTemplatePath.u8string().c_str(), "r");

	char readBuffer[65536];
	rj::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));

	rj::Document doc;
	doc.ParseStream(readStream);

	fclose(file);

	rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator = doc.GetAllocator();

	if (!doc.HasMember("Components"))
	{
		// Case 1: missing → add it
		rj::Value components(rj::kObjectType);
		doc.AddMember("Components", components, allocator);
	}
	else if (!doc["Components"].IsObject()) // TODO: Look into what this does?
	{
		// Case 2: exists but wrong type → fix it
		doc["Components"].SetObject();
	}

	rj::Value& components = doc["Components"];

	for (auto& component : mComponents)
	{
		const char* const compName = component->GetCompName();
		if (!compName || *compName == '\0')
		{
			continue; // TODO: Log that we can't save this comp.
		}

		rj::Value compObj(rj::kObjectType);
		component->SaveComponentToTemplate(compObj, allocator);

		if (components.HasMember(compName))
		{
			components[compName] = compObj;
		}
		else
		{
			components.AddMember(rj::Value(compName, allocator), compObj, allocator);
		}
	}

	fopen_s(&file, mTemplatePath.u8string().c_str(), "w");

	char writeBuffer[65536];
	rj::FileWriteStream writeStream(file, writeBuffer, sizeof(writeBuffer));

	rj::Writer<rj::FileWriteStream> writer(writeStream);
	doc.Accept(writer);

	fclose(file);

	// TODO: Also save pathing to children game objects
}

GameObject* GameObject::GetParentGameObject() const
{
	return mWorld->GetGameObject(mParentGameObjectHandle);
}

void GameObject::SetParent(const std::string& name)
{
	if (name.empty())
	{
		return;
	}

	if (GameObject* parentObject = mWorld->FindGameObject(name))
	{
		SetParent(parentObject);
	}
}

void GameObject::SetParent(GameObject* parentObject)
{
	if (parentObject == nullptr)
	{
		return;
	}

	SetParent(parentObject->GetHandle());
}

void GameObject::SetParent(GameObjectHandle parentObjectHandle)
{
	if (GameObject* oldParentGameObject = mWorld->GetGameObject(mParentGameObjectHandle))
	{
		oldParentGameObject->RemoveChild(mHandle);
	}

	mParentGameObjectHandle = parentObjectHandle;

	if (GameObject* newParentGameObject = mWorld->GetGameObject(mParentGameObjectHandle))
	{
		newParentGameObject->AddChild(mHandle);
	}
}

void GameObject::AddChild(GameObjectHandle childObjectHandle)
{	
	if (mWorld->GetGameObject(childObjectHandle) == nullptr)
	{
		return;
	}

	mChildGameObjectHandles.push_back(childObjectHandle);
}

void GameObject::RemoveChild(GameObjectHandle childObjectHandle)
{
	mChildGameObjectHandles.erase(std::remove(mChildGameObjectHandles.begin(), mChildGameObjectHandles.end(), childObjectHandle), mChildGameObjectHandles.end()); // Erase-Remove Idiom
}
