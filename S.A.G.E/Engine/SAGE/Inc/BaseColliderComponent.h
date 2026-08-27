#pragma once
#include "Component.h"

namespace SAGE
{
	class RBPhysicsService;
	class MeshFilterComponent;
	class RigidBodyComponent;
	class TransformComponent;

	class BaseColliderComponent : public Component
	{
	public:
		void LoadComponentFromTemplate(const rapidjson::Value& value) override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		void Initialize() override;
		void Terminate() override;

		void OnEnable() override;
		void OnDisable() override;

		// Getters
		bool IsTrigger() const { return mIsTrigger; }
		const Math::Vector3 GetCenter() const;
		const Math::Quaternion GetOrientation() const;
		RBPhysics::RBPhysicsObject* GetPhysicsObject() const { return mPhysicsObject; }

		// Setters
		void SetIsTrigger(bool isTrigger);
		void SetCenter(const Math::Vector3& center) { mCenter = center; }

	protected:
		void UpdatePhysicsObjectPropertys();
		virtual std::unique_ptr<RBPhysics::Collider> CreateCollider() = 0;
		virtual void ResizeToMesh() {};

		void NotifyParentOnTriggerEnter(RBPhysics::Collider* collider);
		void NotifyParentOnTriggerStay(RBPhysics::Collider* collider);
		void NotifyParentOnTriggerExit(RBPhysics::Collider* collider);
		void OnTransformPositionChanged(const Math::Vector3& position);
		void OnTransformRotationChanged(const Math::Quaternion& rotation);
		void OnPhysicsObjectPositionChanged(const Math::Vector3& position);
		void OnPhysicsObjectRotationChanged(const Math::Quaternion& rotation);

		RBPhysicsService* mPhysicsService = nullptr;
		TransformComponent* mTransformComponent = nullptr;
		RigidBodyComponent* mRigidBodyComponent = nullptr;
		MeshFilterComponent* mMeshFilterComponent = nullptr;
		RBPhysics::RBPhysicsObject* mPhysicsObject = nullptr;
		
		bool mIsTrigger = false;
		// TODO: Physics Material
		Math::Vector3 mCenter = Math::Vector3::Zero; // TODO: Offset.

		bool mDebugFill = false;
		Graphics::Color mDebugColor = Graphics::Colors::Green;

		Core::Delegate::FDelegateHandle OnTriggerEnterHandle;
		Core::Delegate::FDelegateHandle OnTriggerStayHandle;
		Core::Delegate::FDelegateHandle OnTriggerExitHandle;
		Core::Delegate::FDelegateHandle OnPositionChangedHandle;
		Core::Delegate::FDelegateHandle OnRotationChangedHandle;
	};
}