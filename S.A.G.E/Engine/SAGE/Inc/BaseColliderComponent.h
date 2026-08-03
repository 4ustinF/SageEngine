#pragma once
#include "Component.h"

namespace SAGE
{
	class TransformComponent;

	class BaseColliderComponent : public Component
	{
	public:
		void Initialize() override;
		void Terminate() override;

		// Getters
		bool IsTrigger() const { return mIsTrigger; }
		const Math::Vector3& GetCenter() const { return mCenter; } 

		// Setters
		void SetIsTrigger(bool isTrigger) { mIsTrigger = isTrigger; }
		void SetCenter(const Math::Vector3& center) { mCenter = center; }

	protected:
		TransformComponent* mTransformComponent = nullptr;
		
		bool mIsTrigger = false;
		// TODO: Physics Material
		Math::Vector3 mCenter = Math::Vector3::Zero;
	};
}