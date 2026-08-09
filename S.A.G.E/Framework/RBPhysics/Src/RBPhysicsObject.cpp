#include "Precompiled.h"
#include "RBPhysicsObject.h"

#include "IntersectData.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;

RBPhysicsObject::RBPhysicsObject(const RBPhysicsObject& other) :
	mPosition(other.mPosition),
	mOldPosition(other.mOldPosition),
	mVelocity(other.mVelocity),
	mCollider(other.mCollider),
	mMass(other.mMass)
{
	mCollider->AddReference();
	//mInverseMass = 1.0f / mMass;

	//// This is for sphere body 
	//float I = 2.0f / 5.0f * mMass * mMass * 1.0f; // Radius
	//mInertia = Matrix3(
	//I, 0.0f, 0.0f, 
	//0.0f, I, 0.0f,
	//0.0f, 0.0f, I);

	//mInverseInertia = Inverse(mInertia);
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

void RBPhysicsObject::DebugDraw(bool fillDebugShapes)
{
	if (mCollider)
	{
		mCollider->DebugDraw(mOrientation, fillDebugShapes);
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
	//mVelocity += mAcceleration * deltaTime;
	mPosition += mVelocity * deltaTime;

	// TODO:
	//// Angular integration
	//mAngularVelocity += mAngularAcceleration * deltaTime;

	//// Update orientation via quaternion derivative: q' = 0.5 * q * w (w = [0, ω])
	//Quaternion w(0.0f, mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z);
	//Quaternion deltaOrientation = mOrientation * w * 0.5f;// * deltaTime;

	//mOrientation = Normalize(mOrientation + deltaOrientation);

	// Clear accumulators (forces/torques should be reapplied each frame)
	mAngularAcceleration = Vector3::Zero;
}

void RBPhysicsObject::ResolveCollision(const RBPhysicsObject& otherObject, const IntersectData& intersectData)
{
	const Vector3 normal = intersectData.GetNormal();
	mPosition += normal * intersectData.GetPenetration(); // If other is static...

	// If they both can move - Heavier objects move less
	float invMassSphere = 1.0f / mMass;
	float invMassBox = 1.0f / otherObject.GetMass();

	//float totalInvMass = invMassSphere + invMassBox;
	//mPosition += normal * intersectData.GetPenetration() * (invMassSphere / totalInvMass); // If other is not static
	//otherPos -= normal * penetration * (invMassBox / totalInvMass);

	// Velocity
	Vector3 relativeVelocity = mVelocity - otherObject.GetVelocity(); // Relative velocity:

	float velAlongNormal = Dot(relativeVelocity, normal); // Velocity along the collision normal
	if (velAlongNormal > 0.0f) // If they're already separating:
	{
		return;
	}

	float e = 0.5f; // 0 = no bounce, 1 = perfect bounce // TODO: Adjust 

	// Impulse magnitude:
	float j = -(1.0f + e) * velAlongNormal / (invMassSphere + otherObject.GetMass());

	// Impulse vector:
	Vector3 impulse = j * normal;

	////Apply:
	//sphereVelocity += impulse * invMassSphere;
	//boxVelocity -= impulse * invMassBox;

	ApplyForce(-impulse * invMassSphere);
}

// Force applies acceleration directly (existing behavior kept)
void RBPhysicsObject::ApplyForce(const Vector3& force)
{
	mVelocity += force;
}

void RBPhysicsObject::ApplyImpulse(const Vector3& impulse)
{
	mVelocity += impulse * mInverseMass;
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
