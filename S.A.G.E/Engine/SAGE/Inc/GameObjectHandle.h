#pragma once

#include "Common.h"

namespace SAGE
{
	class GameObjectHandle
	{
	public:
		GameObjectHandle() = default;
		bool operator==(const GameObjectHandle& h) const noexcept { return mIndex == h.mGeneration && mGeneration == h.mGeneration; }

	private:
		friend class GameWorld;

		int mIndex = -1;
		int mGeneration = -1;
	};
}