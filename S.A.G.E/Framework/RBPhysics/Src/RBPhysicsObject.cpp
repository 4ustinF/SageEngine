#include "Precompiled.h"
#include "RBPhysicsObject.h"

using namespace SAGE;
using namespace SAGE::RBPhysics;

void RBPhysicsObject::Integrate(float deltaTime)
{
	mPosition += mVelocity * deltaTime;
}