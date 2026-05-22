#include "Precompiled.h"
#include "RBPhysicsObject.h"

using namespace SAGE;
using namespace SAGE::RBPhysics;

RBPhysicsObject::RBPhysicsObject(const RBPhysicsObject& other) :
	mPosition(other.mPosition),
	mOldPosition(other.mOldPosition),
	mVelocity(other.mVelocity),
	mCollider(other.mCollider)
{
	mCollider->AddReference();
}

void RBPhysicsObject::operator=(RBPhysicsObject other) // TODO: Do this in a better fashion.
{
	char* temp[sizeof(RBPhysicsObject) / sizeof(char)];
	memcpy(temp, this, sizeof(RBPhysicsObject));
	memcpy(this, &other, sizeof(RBPhysicsObject));
	memcpy(&other, temp, sizeof(RBPhysicsObject));
}

RBPhysicsObject::~RBPhysicsObject()
{
	if (mCollider && mCollider->RemoveReference())
	{
		delete mCollider;
	}
}

void RBPhysicsObject::Integrate(float deltaTime)
{
	mPosition += mVelocity * deltaTime;
}