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

		virtual const char* GetCompName() { return "Transform Component"; }

		void DebugUI() override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		void SetPosition(const SAGE::Math::Vector3& pos);
	};
}