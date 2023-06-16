#include "Precompiled.h"
#include "GameObject.h"

using namespace SAGE;

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

	for (auto& component : mComponents) {
		component->Update(deltaTime);
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
