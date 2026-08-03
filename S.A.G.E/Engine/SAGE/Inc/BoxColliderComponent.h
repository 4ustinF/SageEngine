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

		void Initialize() override;
		void Terminate() override;
		void DebugUI() override;

		// Getters
		const Math::Vector3& GetSize() const { return mSize; }

		// Setters
		void SetSize(const Math::Vector3& size);

	private:
		Math::Vector3 mSize = Math::Vector3::One;
	};
}