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

		virtual const char* GetCompName() { return "Transform Component"; }

		void DebugUI() override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		// Getters
		const SAGE::Graphics::Transform& GetTransform() const { return mTransform; }
		SAGE::Graphics::Transform GetTransform() { return mTransform; }
		const SAGE::Math::Vector3& GetPosition() const { return mTransform.position; }
		const SAGE::Math::Vector3& GetRotationDegreeAngles() const { return mDegreeAngles; }
		const SAGE::Math::Quaternion& GetRotation() const { return mTransform.rotation; }
		const SAGE::Math::Vector3& GetScale() const { return mTransform.scale; }
		const SAGE::Math::Matrix4 GetMatrix4() const { return mTransform.GetMatrix4(); }

		const SAGE::Math::Vector3& GetLocalPosition() const { return mLocalTransform.position; }

		FOnPositionChange& GetOnPositionChangeDelegate() { return mOnPositionChange; }
		FOnScaleChange& GetOnScaleChangeDelegate() { return mOnScaleChange; }

		// Setters
		void SetPosition(const SAGE::Math::Vector3& inPos);
		void SetLocalPosition(const SAGE::Math::Vector3& inPos);

		void SetRotation(const SAGE::Math::Vector3& inRotation);
		void SetRotation(const SAGE::Math::Quaternion& inRotation);
		void SetLocalRotation(const SAGE::Math::Vector3& inRotation);
		void SetLocalRotation(const SAGE::Math::Quaternion& inRotation);

		void SetScale(const SAGE::Math::Vector3& inScale);
		void SetLocalScale(const SAGE::Math::Vector3& inScale);

		// Other
		void UpdateRecursivePosition(const GameObjectHandle& gameObjectHandle, const SAGE::Math::Vector3& inWorldPos);

	private:
		SAGE::Graphics::Transform mTransform;
		SAGE::Graphics::Transform mLocalTransform;
		SAGE::Math::Vector3 mDegreeAngles = SAGE::Math::Vector3::Zero;
		SAGE::Math::Vector3 mLocalDegreeAngles = SAGE::Math::Vector3::Zero;

		FOnPositionChange mOnPositionChange;
		FOnScaleChange mOnScaleChange;
	};
}