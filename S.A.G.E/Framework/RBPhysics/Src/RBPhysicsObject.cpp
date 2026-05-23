#include "Precompiled.h"
#include "RBPhysicsObject.h"

using namespace SAGE;
using namespace SAGE::Math;
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

	// This is for sphere body 
	float I = 2.0f / 5.0f * mMass * mMass * 1.0f; // Radius
	mInertia = Matrix3(
	I, 0.0f, 0.0f, 
	0.0f, I, 0.0f,
	0.0f, 0.0f, I);

	mInverseInertia = Inverse(mInertia);
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

void RBPhysicsObject::ApplyForce(const Math::Vector3& force)
{
	mAcceleration += force * mInverseMass;
}

void RBPhysicsObject::ApplyTorque(const Math::Vector3& torque)
{
	// mAngularAcceleration += mInverseInertia * torque; // TODO: Turn this into nice math.
	mAngularAcceleration += Vector3(mInverseInertia._11* torque.x + mInverseInertia._12 * torque.y + mInverseInertia._13 * torque.z,
	mInverseInertia._21* torque.x + mInverseInertia._22 * torque.y + mInverseInertia._23 * torque.z,
	mInverseInertia._31* torque.x + mInverseInertia._32 * torque.y + mInverseInertia._33 * torque.z);
}