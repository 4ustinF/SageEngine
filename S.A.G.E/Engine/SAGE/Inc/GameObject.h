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
		void PreSeedSetActive(bool isSelfActive, bool isActiveInHierarchy);
		void SetActive(bool active);
		void UpdateActiveInHierarchy();

		template<class ComponentType>
		ComponentType* AddComponent()
		{
			ASSERT(!mInitialize, "GameObject - can only add components before object is initialized.");
			auto& newComponent = mComponents.emplace_back(std::make_unique<ComponentType>());
			newComponent->mOwner = this;
			return static_cast<ComponentType*>(newComponent.get());

	/*		ASSERT(!mInitialize, "GameObject - can only add components before object is initialized.");
			auto newComp = std::make_unique<ComponentType>();
			ASSERT(newComp != nullptr, "GameObject - AddComponent allocation failed (out of memory in custom allocator?)");
			auto& stored = mComponents.emplace_back(std::move(newComp));
			stored->mOwner = this;
			return static_cast<ComponentType*>(stored.get());*/
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

		//template <class ComponentType>
		//ComponentType* GetComponentInChildren()
		//{
		//	auto constMe = static_cast<const GameObject*>(this);
		//	return const_cast<ComponentType*>(constMe->GetComponentInChildren<ComponentType>());
		//}

		//template <class ComponentType>
		//const ComponentType* GetComponentInChildren() const // TODO: Might need to make this recursive. 
		//{
		//	GameWorld& world = GetWorld();
		//	for (const GameObjectHandle& handle : mChildGameObjectHandles)
		//	{
		//		if (GameObject* childObj = world.GetGameObject(handle)) // Requires world logic :(
		//		{
		//			if (const ComponentType* component = childObj->GetComponent<ComponentType>())
		//			{
		//				return component;
		//			}
		//		}
		//	}

		//	return nullptr;
		//}

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
		GameObject* GetChildByIndex(int index);
		GameObject* FindChildByName(const std::string& name);
		GameObject* FindDeepChildByName(const std::string& name);

		template <class ComponentType>
		ComponentType* GetComponentInChildren(bool includeInactive = true)
		{
			if (!includeInactive && !mActiveInHierarchy)
			{
				return nullptr; // Inactive objects have inactive children too, so skip the subtree.
			}

			if (ComponentType* component = GetComponent<ComponentType>())
			{
				return component;
			}

			const int childCount = static_cast<int>(mChildGameObjectHandles.size());
			for (int i = 0; i < childCount; ++i)
			{
				if (GameObject* child = GetChildByIndex(i))
				{
					if (ComponentType* found = child->GetComponentInChildren<ComponentType>(includeInactive))
					{
						return found;
					}
				}
			}

			return nullptr;
		}

		template <class ComponentType>
		void GetComponentsInChildren(std::vector<ComponentType*>& outComponents, bool includeInactive = true)
		{
			if (!includeInactive && !mActiveInHierarchy)
			{
				return;
			}

			if (ComponentType* component = GetComponent<ComponentType>())
			{
				outComponents.push_back(component);
			}

			const int childCount = static_cast<int>(mChildGameObjectHandles.size());
			for (int i = 0; i < childCount; ++i)
			{
				if (GameObject* child = GetChildByIndex(i))
				{
					child->GetComponentsInChildren<ComponentType>(outComponents, includeInactive);
				}
			}
		}

		template <class ComponentType>
		std::vector<ComponentType*> GetComponentsInChildren(bool includeInactive = true)
		{
			std::vector<ComponentType*> results;
			GetComponentsInChildren<ComponentType>(results, includeInactive);
			return results;
		}

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