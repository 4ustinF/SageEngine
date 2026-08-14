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
	mCollider->AddReference();
	mInverseMass = 1.0f / mMass;

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

//void RBPhysicsObject::ResolveCollision(const RBPhysicsObject& otherObject, const IntersectData& intersectData)
//{
//	const Vector3 normal = intersectData.GetNormal();
//	mPosition += normal * intersectData.GetPenetration(); // If other is static...
//
//	// If they both can move - Heavier objects move less
//	float invMassSphere = 1.0f / mMass;
//	float invMassBox = 1.0f / otherObject.GetMass();
//
//	//float totalInvMass = invMassSphere + invMassBox;
//	//mPosition += normal * intersectData.GetPenetration() * (invMassSphere / totalInvMass); // If other is not static
//	//otherPos -= normal * penetration * (invMassBox / totalInvMass);
//
//	// Velocity
//	Vector3 relativeVelocity = mVelocity - otherObject.GetVelocity(); // Relative velocity:
//
//	float velAlongNormal = Dot(relativeVelocity, normal); // Velocity along the collision normal
//	if (velAlongNormal > 0.0f) // If they're already separating:
//	{
//		return;
//	}
//
//	float e = 0.5f; // 0 = no bounce, 1 = perfect bounce // TODO: Adjust 
//
//	// Impulse magnitude:
//	float j = -(1.0f + e) * velAlongNormal / (invMassSphere + otherObject.GetMass());
//
//	// Impulse vector:
//	Vector3 impulse = j * normal;
//
//	////Apply:
//	//sphereVelocity += impulse * invMassSphere;
//	//boxVelocity -= impulse * invMassBox;
//
//	ApplyForce(-impulse * invMassSphere);
//}

void RBPhysicsObject::ResolveCollision(const RBPhysicsObject& otherObject, const IntersectData& intersectData)
{
	const Vector3 normal = intersectData.GetNormal();
	const float penetration = intersectData.GetPenetration();

	const float totalInvMass = mInverseMass + otherObject.GetInverseMass();
	//if (totalInvMass <= 0.0f)
	//	return; // TODO: both static/infinite mass, nothing to resolve

	// 1. Positional correction
	const float myShare = mInverseMass / totalInvMass;
	const Vector3 newPos = mPosition + normal * (penetration * myShare); // * 0.8f); = Baumgarte stabilization
	SetPosition(newPos);

	// 2. Velocity response
	const Vector3 relativeVelocity = GetVelocity() - otherObject.GetVelocity();
	const float vn = Dot(relativeVelocity, normal);

	if (vn < 0.0f)
	{
		const float restitution = 0.0f; // Bouncieness
		const float j = -(1.0f + restitution) * vn / totalInvMass;
		const Vector3 impulse = normal * j;

		// Only apply my share of the impulse to myself
		ApplyImpulse(impulse);
	}

	// 3. Ground friction (tangential direction)
	const Vector3 vNormalPart = vn * normal;
	const Vector3 relativeTangent = relativeVelocity - vNormalPart;

	const float tangentSpeed = Magnitude(relativeTangent);
	if (tangentSpeed > 0.0001f)
	{
		Vector3 tangentDir = relativeTangent / tangentSpeed;
		float groundFriction = 0.1f; // tune this, 0 = ice, 1 = very grippy

		float jt = -tangentSpeed * groundFriction / totalInvMass;
		Vector3 frictionImpulse = tangentDir * jt;

		ApplyImpulse(frictionImpulse);
	}
}

void RBPhysicsObject::ResolveCollision(const IntersectData& intersectData)
{
	const Vector3& normal = intersectData.GetNormal();
	const float penetration = intersectData.GetPenetration();
	const Vector3& velocity = GetVelocity();

	// 1. Positional correction
	//const float correctionAmount = std::max(penetration - 0.01f, 0.0f) * 0.2f;
	//const Vector3 newPos = mPosition + normal * correctionAmount; // * 0.8f); = Baumgarte stabilization
	const Vector3 newPos = mPosition + normal * penetration; // * 0.8f); = Baumgarte stabilization
	SetPosition(newPos);

	// 2. Velocity response
	const float vn = Dot(velocity, normal);

	if (vn < 0.0f)
	{
		const float restitution = 0.0f; // Bouncieness
		const float j = -(1.0f + restitution) * vn / mInverseMass;
		const Vector3 impulse = normal * j;

		// Only apply my share of the impulse to myself
		ApplyImpulse(impulse);
	}

	// 3. Ground friction (tangential direction)
	const Vector3 vNormalPart = vn * normal;
	const Vector3 vTangent = velocity - vNormalPart;

	// Cosine of maximum walkable slope angle.
	// 0.7071 ~= cos(45 degrees)
	const float maxSlopeDot = 0.7071f; // TODO: Allow user to adjust.
	const float surfaceUpDot = Dot(normal, Vector3::YAxis);

	if (surfaceUpDot >= maxSlopeDot)
	{
		// Tangential velocity relative to the surface
		const Vector3 vNormalPart = vn * normal;
		const Vector3 vTangent = velocity - vNormalPart;

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
