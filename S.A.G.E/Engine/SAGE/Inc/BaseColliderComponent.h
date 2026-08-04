#pragma once
#include "Component.h"

namespace SAGE
{
	class TransformComponent;

	class BaseColliderComponent : public Component
	{
	public:
		void LoadComponentFromTemplate(const rapidjson::Value& value) override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		void Initialize() override;
		void Terminate() override;

		// Getters
		bool IsTrigger() const { return mIsTrigger; }
		const Math::Vector3 GetCenter() const;

		// Setters
		void SetIsTrigger(bool isTrigger) { mIsTrigger = isTrigger; }
		void SetCenter(const Math::Vector3& center) { mCenter = center; }

	protected:
		TransformComponent* mTransformComponent = nullptr;
		SAGE::RBPhysics::RBPhysicsObject* mPhysicsObject = nullptr;
		
		bool mIsTrigger = false;
		// TODO: Physics Material
		Math::Vector3 mCenter = Math::Vector3::Zero;

		bool mDebugFill = false;
		Graphics::Color mDebugColor = Graphics::Colors::Green;
	};
}