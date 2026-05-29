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

Vector3 RBPhysicsObject::GetLocalPosition(const Math::Vector3& worldPos)
{
	return Conjugate(mOrientation) * (worldPos - mPosition);
}

Vector3 RBPhysicsObject::GetWorldPosition(const Math::Vector3& localPos)
{
	return mPosition + mOrientation * localPos;
}

void RBPhysicsObject::Integrate(float deltaTime)
{
	// Linear integration
	mVelocity += mAcceleration * deltaTime;
	mPosition += mVelocity * deltaTime;

	// Angular integration
	mAngularVelocity += mAngularAcceleration * deltaTime;

	// Update orientation via quaternion derivative: q' = 0.5 * q * w (w = [0, ω])
	Quaternion w(0.0f, mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z);
	Quaternion deltaOrientation = mOrientation * w;
	deltaOrientation.x *= 0.5f * deltaTime;
	deltaOrientation.y *= 0.5f * deltaTime;
	deltaOrientation.z *= 0.5f * deltaTime;
	deltaOrientation.w *= 0.5f * deltaTime;

	mOrientation = Normalize(mOrientation + deltaOrientation);

	// Clear accumulators (forces/torques should be reapplied each frame)
	mAcceleration = Vector3::Zero;
	mAngularAcceleration = Vector3::Zero;
}

// Force applies acceleration directly (existing behavior kept)
void RBPhysicsObject::ApplyForce(const Vector3& force)
{
	mAcceleration += force * mInverseMass;
}

void RBPhysicsObject::ApplyForceAtPoint(const Vector3& force, const Vector3& localPoint)
{
	const Vector3 localForce = Conjugate(mOrientation) * force;
	const Vector3 torque = Cross(localPoint, localForce);
	ApplyForce(force);
	ApplyTorque(torque);
}

void RBPhysicsObject::ApplyTorque(const Vector3& torque)
{
	// mAngularAcceleration += mInverseInertia * torque; // TODO: Turn this into nice math.
	mAngularAcceleration += QuatMulVec3(torque, mInverseInertia);
}

void RBPhysicsObject::ApplyDrag(const Vector3& velocity, const Vector3& dragForce)
{
	// dragForce is expected to be a force (N). Apply as linear force.
	ApplyForce(dragForce);

	// Simple angular damping proportional to linear drag magnitude (hacky but works for demo)
	float dragMag = Magnitude(dragForce);
	if (dragMag > 0.0f)
	{
		// Dampen angular acceleration/velocity slightly
		const float angularDampCoef = 0.1f;
		mAngularAcceleration += -mAngularVelocity * (angularDampCoef * dragMag);
	}
}

Math::Vector3 RBPhysicsObject::QuatMulVec3(const Math::Vector3& vec, const Math::Matrix3& m)
{
	return Vector3(m._11 * vec.x + m._12 * vec.y + m._13 * vec.z,
		m._21 * vec.x + m._22 * vec.y + m._23 * vec.z,
		m._31 * vec.x + m._32 * vec.y + m._33 * vec.z);
}
