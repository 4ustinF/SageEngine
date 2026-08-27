#pragma once

#include "Component.h"

#include "GameObjectHandle.h"

namespace SAGE
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPositionChange, const Math::Vector3&);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRotationChange, const Math::Quaternion&);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnScaleChange, const Math::Vector3&);

	class TransformComponent final 
		: public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Transform)
		MEMORY_POOL_DECLARE

		const char* GetCompName() override { return "Transform Component"; }
		void LoadComponentFromTemplate(const rapidjson::Value& value) override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		void Initialize() override;
		void DebugUI() override;

		// Getters
		const Graphics::Transform& GetTransform() const { return mTransform; }
		Graphics::Transform GetTransform() { return mTransform; }
		const Math::Vector3& GetPosition() const { return mTransform.position; }
		const Math::Vector3& GetRotationDegreeAngles() const { return mDegreeAngles; }
		const Math::Quaternion& GetRotation() const { return mTransform.rotation; }
		const Math::Vector3& GetScale() const { return mTransform.scale; }
		const Math::Matrix4 GetMatrix4() const { return mTransform.GetMatrix4(); }

		const Graphics::Transform& GetLocalTransform() const { return mLocalTransform; }
		Graphics::Transform GetLocalTransform() { return mLocalTransform; }
		const Math::Vector3& GetLocalPosition() const { return mLocalTransform.position; }
		const Math::Vector3& GetLocalRotationDegreeAngles() const { return mLocalDegreeAngles; }
		const Math::Quaternion& GetLocalRotation() const { return mLocalTransform.rotation; }
		const Math::Vector3& GetLocalScale() const { return mLocalTransform.scale; }
		const Math::Matrix4 GetLocalMatrix4() const { return mLocalTransform.GetMatrix4(); }

		FOnPositionChange& GetOnPositionChangeDelegate() { return mOnPositionChange; }
		FOnRotationChange& GetOnRotationChangeDelegate() { return mOnRotationChange; }
		FOnScaleChange& GetOnScaleChangeDelegate() { return mOnScaleChange; }

		// Setters
		void SetPosition(const Math::Vector3& inPos);
		void SetRotation(const Math::Vector3& inRotation);
		void SetRotation(const Math::Quaternion& inRotation);
		void SetScale(const Math::Vector3& inScale);

		void SetLocalPosition(const Math::Vector3& inPos);
		void SetLocalRotation(const Math::Vector3& inRotation);
		void SetLocalRotation(const Math::Quaternion& inRotation);
		void SetLocalScale(const Math::Vector3& inScale);

	private:
		const TransformComponent* FindParentTransformComponent() const;
		void UpdateWorldPosition(const Math::Vector3& inPos);
		void UpdateChildrenPositions(const GameObjectHandle& gameObjectHandle, const Math::Vector3& inWorldPos);

		void UpdateWorldRotation(const Math::Quaternion& inRotation);
		void UpdateChildrenRotation(const GameObjectHandle& gameObjectHandle, const Math::Quaternion& inWorldRotation);

		void UpdateWorldScale(const Math::Vector3& inScale);
		void UpdateChildrenScales(const GameObjectHandle& gameObjectHandle, const Math::Vector3& inWorldScale);

		Graphics::Transform mTransform;
		Graphics::Transform mLocalTransform;
		Math::Vector3 mDegreeAngles = Math::Vector3::Zero;
		Math::Vector3 mLocalDegreeAngles = Math::Vector3::Zero;

		FOnPositionChange mOnPositionChange;
		FOnRotationChange mOnRotationChange;
		FOnScaleChange mOnScaleChange;
	};
}