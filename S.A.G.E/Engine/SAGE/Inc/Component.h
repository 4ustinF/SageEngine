#pragma once

#include "TypeIds.h"

namespace SAGE
{
	class GameObject;

	class Component
	{
	public:
		Component() = default;
		virtual ~Component() = default;

		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(Component&&) = delete;

		virtual uint32_t GetTypeId() const = 0;

		virtual const char* GetCompName() { return ""; }
		virtual void LoadComponentFromTemplate(const rapidjson::Value& value) {}
		virtual void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) {} // TODO: We should templatize the save code for each type.

		virtual void Initialize() {}
		virtual void Terminate() {}

		virtual void Update(float deltaTime) {}
		virtual void OnQueueUpdate(float deltaTime) {}
		virtual void DebugUI() {}

		virtual void OnEnable() {}
		virtual void OnDisable() {}

		void EnqueueUpdate() { mQueueUpdate = true; }
		void ClearQueueUpdate() { mQueueUpdate = false; }
		bool CanQueueUpdate() const { return mQueueUpdate; }

		GameObject& GetOwner() { return *mOwner; }
		const GameObject& GetOwner() const { return *mOwner; }

	private:
		friend class GameObject;
		GameObject* mOwner = nullptr;

		bool mQueueUpdate = false;
	};
}