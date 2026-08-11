#pragma once
#include "BaseColliderComponent.h"

namespace SAGE
{
	class TransformComponent;

	class BoxColliderComponent final : public BaseColliderComponent
	{
	public:
		SET_TYPE_ID(ComponentId::BoxCollider)
		MEMORY_POOL_DECLARE

		const char* GetCompName() override { return "Box Collider Component"; }
		void LoadComponentFromTemplate(const rapidjson::Value& value) override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		void Initialize() override;
		void Terminate() override;
		void DebugUI() override;

		void OnEnable() override;

		// Getters
		const Math::Vector3& GetSize() const { return mSize; }

		// Setters
		void SetSize(const Math::Vector3& size);

	protected:
		void ResizeToMesh() override;

	private:
		Math::Vector3 mSize = Math::Vector3::One;
		Math::Vector3 mExtend = Math::Vector3::Half;
	};
}