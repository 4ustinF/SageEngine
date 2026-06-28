#pragma once

#include "Component.h"

#include "GameObjectHandle.h"

namespace SAGE
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPositionChange, SAGE::Math::Vector3);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnScaleChange, const SAGE::Math::Vector3&);

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
		const SAGE::Graphics::Transform& GetTransform() const { return mTransform; }
		SAGE::Graphics::Transform GetTransform() { return mTransform; }
		const SAGE::Math::Vector3& GetPosition() const { return mTransform.position; }
		const SAGE::Math::Vector3& GetRotationDegreeAngles() const { return mDegreeAngles; }
		const SAGE::Math::Quaternion& GetRotation() const { return mTransform.rotation; }
		const SAGE::Math::Vector3& GetScale() const { return mTransform.scale; }
		const SAGE::Math::Matrix4 GetMatrix4() const { return mTransform.GetMatrix4(); }

		const SAGE::Graphics::Transform& GetLocalTransform() const { return mLocalTransform; }
		SAGE::Graphics::Transform GetLocalTransform() { return mLocalTransform; }
		const SAGE::Math::Vector3& GetLocalPosition() const { return mLocalTransform.position; }
		const SAGE::Math::Vector3& GetLocalRotationDegreeAngles() const { return mLocalDegreeAngles; }
		const SAGE::Math::Quaternion& GetLocalRotation() const { return mLocalTransform.rotation; }
		const SAGE::Math::Vector3& GetLocalScale() const { return mLocalTransform.scale; }
		const SAGE::Math::Matrix4 GetLocalMatrix4() const { return mLocalTransform.GetMatrix4(); }

		FOnPositionChange& GetOnPositionChangeDelegate() { return mOnPositionChange; }
		FOnScaleChange& GetOnScaleChangeDelegate() { return mOnScaleChange; }

		// Setters
		void SetPosition(const SAGE::Math::Vector3& inPos);
		void SetRotation(const SAGE::Math::Vector3& inRotation);
		void SetRotation(const SAGE::Math::Quaternion& inRotation);
		void SetScale(const SAGE::Math::Vector3& inScale);

		void SetLocalPosition(const SAGE::Math::Vector3& inPos);
		void SetLocalRotation(const SAGE::Math::Vector3& inRotation);
		void SetLocalRotation(const SAGE::Math::Quaternion& inRotation);
		void SetLocalScale(const SAGE::Math::Vector3& inScale);

	private:
		const TransformComponent* FindParentTransformComponent() const;
		void UpdateWorldPosition(const SAGE::Math::Vector3& inPos);
		void UpdateChildrenPositions(const GameObjectHandle& gameObjectHandle, const SAGE::Math::Vector3& inWorldPos);
		void UpdateWorldScale(const SAGE::Math::Vector3& inScale);
		void UpdateChildrenScales(const GameObjectHandle& gameObjectHandle, const SAGE::Math::Vector3& inWorldScale);

		SAGE::Graphics::Transform mTransform;
		SAGE::Graphics::Transform mLocalTransform;
		SAGE::Math::Vector3 mDegreeAngles = SAGE::Math::Vector3::Zero;
		SAGE::Math::Vector3 mLocalDegreeAngles = SAGE::Math::Vector3::Zero;

		FOnPositionChange mOnPositionChange;
		FOnScaleChange mOnScaleChange;
	};
}