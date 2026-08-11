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
		SAGE::RBPhysics::RBPhysicsObject* GetPhysicsObject() const { return mPhysicsObject; }

		// Setters
		void SetIsTrigger(bool isTrigger) { mIsTrigger = isTrigger; }
		void SetCenter(const Math::Vector3& center) { mCenter = center; }
		void SetOrientation(const Math::Quaternion& orientation) { mOrientation = orientation; }

	protected:
		void UpdatePhysicsObjectPropertys();
		virtual void ResizeToMesh() {};

		RBPhysicsService* mPhysicsService = nullptr;
		TransformComponent* mTransformComponent = nullptr;
		RigidBodyComponent* mRigidBodyComponent = nullptr;
		MeshFilterComponent* mMeshFilterComponent = nullptr;
		SAGE::RBPhysics::RBPhysicsObject* mPhysicsObject = nullptr;
		
		bool mIsTrigger = false;
		// TODO: Physics Material
		Math::Vector3 mCenter = Math::Vector3::Zero;
		Math::Quaternion mOrientation = Math::Quaternion::Identity;

		bool mDebugFill = false;
		Graphics::Color mDebugColor = Graphics::Colors::Green;
	};
}