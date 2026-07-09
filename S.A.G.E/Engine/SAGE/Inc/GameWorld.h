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

		GameObject* CreateGameObjectRecursive(std::filesystem::path templateFile, GameObject* parentGO, const char* overrideName);

		struct RaycastHit
		{
			bool hit = false;
			float distance = 0.0f;
			SAGE::Math::Vector3 position;
			SAGE::Math::Vector3 normal;
			uint32_t triangleIndex = 0;
		};

		bool IntersectRayMesh(const SAGE::Math::Ray& ray, const SAGE::Graphics::Mesh& mesh, RaycastHit& outHit);
		bool IntersectRayTriangle(const SAGE::Math::Ray& ray, const SAGE::Math::Vector3& v0, const SAGE::Math::Vector3& v1, const SAGE::Math::Vector3& v2, float& outDistance, SAGE::Math::Vector3& outNormal);

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
		void DrawGameObjectNode(GameObject* object);

		bool mHierarchyDirty = true;
		std::string mRemainingSectionName = "Other";

		void DrawInspector();
		Service* mInspectorService = nullptr;
		GameObject* mInspectorGameObject = nullptr;

		void DrawAddComponentWindow();
		bool mAddComponentWindowActive = false;

		bool mInitialized = false;
		bool mUpdating = false;
		float mImguiSpacing = 5.0f;

		std::vector<GameObjectHandle> mRootGameObjectHandles;

		int mDebugClickCount1 = 0;
		int mDebugClickCount2 = 0;
	};
}