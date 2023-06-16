#pragma once

#include "Component.h"

namespace SAGE
{
	class TransformComponent final 
		: public Component
		, public Graphics::Transform
	{
	public:
		SET_TYPE_ID(ComponentId::Transform)
		MEMORY_POOL_DECLARE

		void DebugUI() override;

		void SetPosition(const SAGE::Math::Vector3& pos);
	};
}