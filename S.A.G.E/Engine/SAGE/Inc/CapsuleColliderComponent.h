#pragma once
#include "BaseColliderComponent.h"

namespace SAGE
{
	class TransformComponent;

	class CapsuleColliderComponent final : public BaseColliderComponent
	{
	public:
		SET_TYPE_ID(ComponentId::CapsuleCollider)
		MEMORY_POOL_DECLARE

		const char* GetCompName() override { return "Capsule Collider Component"; }

		void Initialize() override;
		void Terminate() override;
		void DebugUI() override;

		// Getters
		float GetRadius() const { return mRadius; }
		float GetHeight() const { return mHeight; }

		// Setters
		void SetRadius(float radius);
		void SetHeight(float height);

	private:
		float mRadius = 1.0f;
		float mHeight = 1.0f;
		// TODO: Direction?
	};
}