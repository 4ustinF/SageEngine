#include "Precompiled.h"
#include "BaseColliderComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;

void BaseColliderComponent ::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
}

void BaseColliderComponent ::Terminate()
{
	mTransformComponent = nullptr;
}
