#pragma once

#include "Component.h"
#include "GameObjectHandle.h"

namespace SAGE
{
	class GameWorld;
	class RBPhysics::Collider;

	class GameObject final
	{
	public:
		MEMORY_POOL_DECLARE

		GameObject() = default;

		void Initialize();
		void Terminate();

		void Update(float deltaTime);
		void DebugUI();

		void OnTriggerEnter(RBPhysics::Collider* collider);
		void OnTriggerStay(RBPhysics::Collider* collider);
		void OnTriggerExit(RBPhysics::Collider* collider);

		const bool IsActiveInHierarchy() const { return mActiveInHierarchy; }
		const bool IsSelfActive() const { return mSelfActive; }
		void SetActive(bool active);
		void UpdateActiveInHierarchy();

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

	protected:
		bool GetIsReparenting() const { return mIsReparenting; }
		void SetIsReparenting(bool isReparenting) { mIsReparenting = isReparenting; }
		bool IsGameObjectAChild(const GameObject* object, const GameObject* child) const;

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
		bool mActiveInHierarchy = true;
		bool mSelfActive = true;
		bool mIsReparenting = false;

		Components mComponents;
		GameWorld* mWorld = nullptr;
		GameObjectHandle mHandle;

		GameObjectHandle mParentGameObjectHandle;
		std::vector<GameObjectHandle> mChildGameObjectHandles;
	};
}