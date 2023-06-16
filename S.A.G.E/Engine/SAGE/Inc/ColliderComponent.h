#pragma once

#include "Component.h"

namespace SAGE
{
	enum class ColliderShapes
	{
		Box,
		Sphere,
		Capsule,
		StaticPlane
	};

	class TransformComponent;

	class ColliderComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Collider)
		MEMORY_POOL_DECLARE

		void Initialize() override;
		void Terminate() override;
		void DebugUI() override;

		Math::AABB GetLocalAABB() const;
		Math::AABB GetWorldAABB() const;
		Math::Vector3 GetExtend() { return mExtend; }

		void SetCenter(const Math::Vector3& center) { mCenter = center; }
		void SetExtend(const Math::Vector3& extend) { mExtend = extend; }
		void SetColliderShape(ColliderShapes shape) { mColliderShapes = shape; } // TODO: Have this function update mCollisionShape

		const Math::Vector3 GetCenter() { return mCenter; }
		btCollisionShape* GetCollisionShape();

	private:
		const TransformComponent* mTransformComponent = nullptr;
		btCollisionShape* mCollisionShape = nullptr;

		Math::Vector3 mCenter = Math::Vector3::Zero;
		Math::Vector3 mExtend = Math::Vector3::One;
		ColliderShapes mColliderShapes = ColliderShapes::Box;
	};
}