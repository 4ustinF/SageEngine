#pragma once

#include "Component.h"

namespace SAGE
{
	class SelectionBoxComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::SelectionBox)
		MEMORY_POOL_DECLARE

		void Initialize() override;
		void Terminate() override;

		void DebugUI() override;

		const Math::OBB GetGlobalBoundingBox();

	private:
		void GenerateGlobalBoundingBox();
		void GatherOBBCorners(const GameObject* gameObject, std::vector<Math::Vector3>& points);

		Math::OBB mBoundingBox;
		bool mHasConstructedBoundingBox = false;
	};
}