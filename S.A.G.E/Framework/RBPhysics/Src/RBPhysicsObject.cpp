#include "Precompiled.h"
#include "RBPhysicsObject.h"

using namespace SAGE;
using namespace SAGE::RBPhysics;

RBPhysicsObject::RBPhysicsObject(const RBPhysicsObject& other) :
	mPosition(other.mPosition),
	mOldPosition(other.mOldPosition),
	mVelocity(other.mVelocity),
	mCollider(other.mCollider),
	mAcceleration(other.mAcceleration)
{
	mCollider->AddReference();
	mMass = 1.0f;
	mInverseMass = 1.0f / mMass;
	mInertia = Math::Matrix3::Identity;
	mInverseInertia = Math::Inverse(mInertia);
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
	if (mPosition.y < 1.0f)
	{
		mPosition.y = 1.0f;
		mVelocity = -mVelocity * 0.85f; // Damping.
	}

	mPosition += mVelocity * deltaTime;
	mVelocity += mAcceleration * deltaTime;
}