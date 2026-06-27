#pragma once

#include "Component.h"
#include "GameObjectHandle.h"

namespace SAGE
{
	class GameWorld;

	class GameObject final
	{
	public:
		MEMORY_POOL_DECLARE

		GameObject() = default;

		void Initialize();
		void Terminate();

		void Update(float deltaTime);
		void DebugUI();

		const bool IsActive() const { return mIsActive; }
		void SetActive(bool active);

		template<class ComponentType>
		ComponentType* AddComponent()
		{
			ASSERT(!mInitialize, "GameObject - can only add components before object is initialized.");
			auto& newComponent = mComponents.emplace_back(std::make_unique<ComponentType>());
			newComponent->mOwner = this;
			return static_cast<ComponentType*>(newComponent.get());
		}

		template <class ComponentType>
		ComponentType* GetComponent()
		{
			auto constMe = static_cast<const GameObject*>(this);
			return const_cast<ComponentType*>(constMe->GetComponent<ComponentType>());
		}

		template <class ComponentType>
		const ComponentType* GetComponent() const
		{
			for (auto& component : mComponents) {
				if (component->GetTypeId() == ComponentType::StaticGetTypeId()) {
					return static_cast<const ComponentType*>(component.get());
				}
			}
			return nullptr;
		}

		GameWorld& GetWorld() { return *mWorld; }
		const GameWorld& GetWorld() const { return *mWorld; }

		GameObjectHandle GetHandle() const { return mHandle; }

		void SetName(const char* name) { mName = name; }
		const std::string& GetName() const { return mName; }

		// New Hierarchy Flow
		GameObjectHandle GetParentHandle() { return mParentGameObjectHandle; }
		GameObject* GetParentGameObject() const;
		void SetParent(const std::string& parentObjectName);
		void SetParent(GameObject* parentObject);
		void SetParent(GameObjectHandle parentObjectHandle);

		void AddChild(GameObjectHandle childObjectHandle);
		void RemoveChild(GameObjectHandle childObjectHandle);
		const std::vector<GameObjectHandle>& GetChildrenHandles() const { return mChildGameObjectHandles; }

	private:
		friend class GameWorld;
		using Components = std::vector<std::unique_ptr<Component>>;

		void OnEnable();
		void OnDisable();
		void SaveComponents();

		std::string mName = "No Name";
		std::filesystem::path mTemplatePath = "";
		void SetTemplatePath(const std::filesystem::path& path) { mTemplatePath = path; }
		bool mInitialize = false;
		bool mIsActive = true;

		Components mComponents;
		GameWorld* mWorld = nullptr;
		GameObjectHandle mHandle;

		GameObjectHandle mParentGameObjectHandle;
		std::vector<GameObjectHandle> mChildGameObjectHandles;
	};
}