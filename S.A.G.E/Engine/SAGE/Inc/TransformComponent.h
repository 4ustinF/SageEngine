#pragma once

#include "Component.h"

namespace SAGE
{
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

		// Setters
		void SetPosition(const SAGE::Math::Vector3& inPos);
		void SetRotation(const SAGE::Math::Vector3& inRotation);
		void SetRotation(const SAGE::Math::Quaternion& inRotation);
		void SetScale(const SAGE::Math::Vector3& inScale);

	private:
		SAGE::Graphics::Transform mTransform;
		SAGE::Math::Vector3 mDegreeAngles = SAGE::Math::Vector3::Zero;
	};
}