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
		virtual void PreSaveComponentToTemplate() {}
		virtual void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) {} // TODO: We should templatize the save code for each type.
		virtual void PostSaveComponentToTemplate() {}

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

	protected:
		std::string OpenFileDialog(const char* fileFilterType);

		void SaveBoolToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, bool value);
		void SaveStringToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const std::string& value);
		void SaveStringsToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const std::vector<std::string>& values);
		void SaveNumberToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, int value, int defaultValue = 0);
		void SaveNumberToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, float value, float defaultValue = 0.0f);
		void SaveColorToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const SAGE::Graphics::Color& value, const SAGE::Graphics::Color& defaultValue = SAGE::Graphics::Colors::Black);
		void SaveVector2ToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const SAGE::Math::Vector2& value, const SAGE::Math::Vector2& defaultValue = SAGE::Math::Vector2::Zero);
		void SaveVector2IntToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const SAGE::Math::Vector2Int& value, const SAGE::Math::Vector2Int& defaultValue = SAGE::Math::Vector2Int::Zero);
		void SaveVector3ToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const SAGE::Math::Vector3& value, const SAGE::Math::Vector3& defaultValue = SAGE::Math::Vector3::Zero);
		void SaveVector4ToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const SAGE::Math::Vector4& value, const SAGE::Math::Vector4& defaultValue = SAGE::Math::Vector4::Zero);

		// TODO: Add some load functions as well that return a bool and provide an out param of the value.

	private:
		friend class GameObject;
		GameObject* mOwner = nullptr;

		bool mQueueUpdate = false;
	};
}