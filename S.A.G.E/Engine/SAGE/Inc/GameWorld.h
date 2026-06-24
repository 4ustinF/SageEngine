#pragma once

#include "GameObject.h"
#include "Service.h"

namespace SAGE
{
	class GameWorld final
	{
	public:
		void Initialize(uint32_t capacity);
		void Terminate();

		void Update(float deltaTime);
		void Render();
		void DebugUI();

		template <class ServiceType>
		ServiceType* AddService()
		{
			ASSERT(!mInitialized, "GameWorld - can only add service before world is initialized.");
			auto& newService = mServices.emplace_back(std::make_unique<ServiceType>());
			newService->mWorld = this;
			return static_cast<ServiceType*>(newService.get());
		}

		template <class ServiceType>
		ServiceType* GetService()
		{
			for (auto& service : mServices) {
				if (service->GetTypeId() == ServiceType::StaticGetTypeId()) {
					return static_cast<ServiceType*>(service.get());
				}
			}
			return nullptr;
		}

		void LoadLevel(std::filesystem::path levelFile);
		GameObject* CreateGameObject(std::filesystem::path templateFile);
		GameObject* FindGameObject(const std::string& name);
		GameObject* GetGameObject(GameObjectHandle handle);
		void DestroyGameObject(GameObjectHandle handle);

		// TODO:
		GameObject* CreateGameObjectRecursive(std::filesystem::path templateFile, GameObject* parentGO, const char* overrideName);

	private:
		bool IsValid(GameObjectHandle handle) const;
		void ProcessDestroyList();

		struct Slot
		{
			std::unique_ptr<GameObject> gameObject;
			uint32_t generation = 0;
		};

		using Services = std::vector<std::unique_ptr<Service>>;
		using GameObjectSlots = std::vector<Slot>;
		using GameObjectPtrs = std::vector<GameObject*>;

		Services mServices;

		GameObjectSlots mGameObjectSlots;
		std::vector<uint32_t> mFreeSlots;
		std::vector<uint32_t> mToBeDestroyed;

		GameObjectPtrs mUpdateList;

		void RebuildHierarchy();
		void DrawHierarchy();

		// TODO: Allow for item renaming using something like ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)
		std::vector<GameObject*> GetRootObjects();
		void DrawGameObjectNode(GameObject* object);
		std::vector<GameObjectHandle> mRootObjectHandles;

		bool mHierarchyDirty = true;
		std::string mRemainingSectionName = "Other";

		void DrawInspector();
		Service* mInspectorService = nullptr;
		GameObject* mInspectorGameObject = nullptr;

		void LoadComponentNames();
		void DrawAddComponentWindow();
		bool mAddComponentWindowActive = false;
		std::vector<const char*> mComponentNames;

		bool mInitialized = false;
		bool mUpdating = false;
		float mImguiSpacing = 5.0f;

	};
}