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
	mMass(other.mMass),
	mOrientation(other.mOrientation)
{
	mInverseMass = mMass != 0.0f ? 1.0f / mMass : 0.0f; // static objects often have mass 0
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
	mCollider = nullptr;
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
	mPosition += mVelocity * deltaTime;

	// Clear accumulators (forces/torques should be reapplied each frame)
	mAngularAcceleration = Vector3::Zero;

	const Vector3 translation = mPosition - mOldPosition;
	mOldPosition = mPosition;
	mCollider->Transform(translation);
}

void RBPhysicsObject::ResolveCollision(const RBPhysicsObject& otherObject, const IntersectData& intersectData)
{
	ResolveCollisionInternal(otherObject.GetVelocity(), otherObject.GetInverseMass(), intersectData);
}

void RBPhysicsObject::ResolveCollision(const IntersectData& intersectData)
{
	// Treat as colliding with an immovable, stationary object (infinite mass).
	ResolveCollisionInternal(Vector3::Zero, 0.0f, intersectData);
}

void RBPhysicsObject::ResolveCollisionInternal(const Vector3& otherVelocity, float otherInverseMass, const IntersectData& intersectData)
{
	const Vector3 normal = intersectData.GetNormal();
	const float penetration = intersectData.GetPenetration();
	const float totalInvMass = mInverseMass + otherInverseMass;

	if (totalInvMass <= 0.0f)
	{
		// Both objects infinite mass - nothing to resolve.
		return;
	}

	// 1. Positional correction
	const float myShare = mInverseMass / totalInvMass;
	const Vector3 newPos = mPosition + normal * (penetration * myShare); // * 0.8f); = Baumgarte stabilization
	SetPosition(newPos);

	// 2. Velocity response
	const Vector3 relativeVelocity = GetVelocity() - otherVelocity;
	const float vn = Dot(relativeVelocity, normal);

	if (vn < 0.0f)
	{
		const float restitution = 0.0f; // Bouncieness
		const float j = -(1.0f + restitution) * vn / totalInvMass;
		const Vector3 impulse = normal * j;

		// Only apply my share of the impulse to myself
		ApplyImpulse(impulse);
	}

	// Cosine of maximum walkable slope angle.
	const float maxSlopeDot = 0.7071f; // TODO: Get from physics world settings.
	const float surfaceUpDot = Dot(normal, Vector3::YAxis);

	if (surfaceUpDot >= maxSlopeDot)
	{
		// Tangential velocity relative to the surface
		const Vector3 vNormalPart = vn * normal;
		const Vector3 vTangent = relativeVelocity - vNormalPart;

		const float tangentSpeed = Magnitude(vTangent);

		if (tangentSpeed > 0.0001f)
		{
			const Vector3 tangentDir = vTangent / tangentSpeed;
			const float groundFriction = 0.1f; // TODO: Tune this, 0 = ice, 1 = very grippy
			const float jt = -tangentSpeed * groundFriction / mInverseMass;
			const Vector3 frictionImpulse = tangentDir * jt;

			ApplyImpulse(frictionImpulse);
		}
	}
}

void RBPhysicsObject::ApplyForce(const Vector3& force)
{
	mVelocity += force;
}

void RBPhysicsObject::ApplyImpulse(const Vector3& impulse)
{
	mVelocity += impulse * mInverseMass;
}
