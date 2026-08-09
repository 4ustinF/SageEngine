#include "Precompiled.h"
#include "RBPhysicsWorld.h"

#include "IntersectData.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;

void RBPhysicsWorld::Initialize(Settings settings)
{
	mSettings = std::move(settings);
}

void RBPhysicsWorld::Clear()
{
	mDynamicObjects.clear();
	mStaticObjects.clear();
}

void RBPhysicsWorld::Update(float deltaTime)
{
	Simulate(deltaTime);
	HandleCollisions();

	//DetectCollisionWithDome(deltaTime);
}

void RBPhysicsWorld::DrawPhysicsObjects(bool fillShapes)
{
	for (RBPhysicsObject& object : mDynamicObjects)
	{
		object.DebugDraw(fillShapes);
	}

	for (RBPhysicsObject& object : mStaticObjects)
	{
		object.DebugDraw(fillShapes);
	}
}

//void RBPhysicsWorld::DebugUI()
//{
//	if (mDynamicObjects.empty())
//		return;
//
//	ImGui::Begin("Physics", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
//	ImGui::DragFloat3("Gravity", &mSettings.gravity.x, 0.1f, -10.0f, 10.0f);
//
//	if (!mDynamicObjects.empty())
//	{
//		auto& o = mDynamicObjects[0];
//		ImGui::Text("Obj0 Pos: %.2f, %.2f, %.2f", o.GetPosition().x, o.GetPosition().y, o.GetPosition().z);
//		ImGui::Text("Obj0 Vel: %.2f, %.2f, %.2f", o.GetVelocity().x, o.GetVelocity().y, o.GetVelocity().z);
//		ImGui::Text("Obj0 Acc: %.3f, %.3f, %.3f", o.GetAcceleration().x, o.GetAcceleration().y, o.GetAcceleration().z);
//		Quaternion orientation = o.GetOrientation();
//		ImGui::DragFloat4("Orientation", &orientation.x, 0.01f, -10000.0f, 10000.0f); // TODO: Remove
//	}
//
//	ImGui::End();
//}

RBPhysicsObject* RBPhysicsWorld::AddObject(const RBPhysicsObject& object, PhysicsObjectType type /*= PhysicsObjectType::Static*/)
{
	switch (type)
	{
	case PhysicsObjectType::Dynamic:
	case PhysicsObjectType::Kinematic: // TODO: 
		mDynamicObjects.push_back(object);
		return &mDynamicObjects.back();
		break;
	case PhysicsObjectType::Static:
		mStaticObjects.push_back(object);
		return &mStaticObjects.back();
	}

	return nullptr; // TODO: This function does not need to return an index. Remove this. 
	// TODO: Maybe return the object pointer instead. Or return a bool for success/failure. Or return nothing.
}

bool  RBPhysicsWorld::RemoveObject(const RBPhysicsObject& object)
{
	// TODO: 
	//auto it = std::find(mDynamicObjects.begin(), mDynamicObjects.end(), object);
	//if (it != mDynamicObjects.end())
	//{
	//	mDynamicObjects.erase(it);
	//	return true;
	//}

	//it = std::find(mStaticObjects.begin(), mStaticObjects.end(), object);
	//if (it != mStaticObjects.end())
	//{
	//	mStaticObjects.erase(it);
	//	return true;
	//}

	return false;
}

void RBPhysicsWorld::Simulate(float deltaTime)
{
	//for (RBPhysicsObject& object : mDynamicObjects)
	//{
	//	// Apply gravity to acceleration accumulator
	//	Vector3 acceleration = object.GetAcceleration() + mSettings.gravity;
	//	object.SetAcceleration(acceleration);

	//	// Compute simple air drag (force)
	//	Vector3 dragForce = -object.GetVelocity() * mSettings.airDragCoeficient * 0.5f;
	//	float magnitude = Magnitude(dragForce);

	//	if (magnitude > 0.0f)
	//	{
	//		if (magnitude > mSettings.maxAirdrag)
	//		{
	//			dragForce /= magnitude;
	//			dragForce *= mSettings.maxAirdrag;
	//		}

	//		// Apply drag as a force (Integrate will use acceleration)
	//		object.ApplyDrag(object.GetVelocity(), dragForce);
	//	}

	//	// Integrate linear and angular motion (RBPhysicsObject::Integrate clears accumulators)
	//	object.Integrate(deltaTime);
	//}

	for (RBPhysicsObject& object : mDynamicObjects)
	{
		object.ApplyForce(mSettings.gravity * deltaTime);
		object.Integrate(deltaTime);
	}
}

void RBPhysicsWorld::HandleCollisions()
{
	const int objectsCount = GetObjectsCount();
	for (int primaryIndex = 0; primaryIndex < objectsCount; ++primaryIndex)
	{
		RBPhysicsObject& primaryObject = mDynamicObjects[primaryIndex];
		for (int secondaryIndex = primaryIndex + 1; secondaryIndex < objectsCount; ++secondaryIndex)
		{
			RBPhysicsObject& secondaryObject = mDynamicObjects[secondaryIndex];
			IntersectData intersectData = primaryObject.GetCollider().Intersect(secondaryObject.GetCollider());

			if (intersectData.GetDoesIntersect())
			{
				// Old sphere vs sphere logic
				//const Vector3 direction = Normalize(intersectData.GetDirection());
				//const Vector3 primVel = primaryObject.GetVelocity();
				//const Vector3 otherDirection = Reflect(direction, Normalize(primVel));
				//
				//primaryObject.SetVelocity(Reflect(primVel, otherDirection));
				//secondaryObject.SetVelocity(Reflect(secondaryObject.GetVelocity(), direction));

				//const Vector3 normal = intersectData.GetNormal();
				//mPosition += normal * intersectData.GetPenetration(); // If other is static...

				//// If they both can move - Heavier objects move less
				//float invMassSphere = 1.0f / mMass;
				//float invMassBox = 1.0f / otherObject.GetMass();

				////float totalInvMass = invMassSphere + invMassBox;
				////mPosition += normal * intersectData.GetPenetration() * (invMassSphere / totalInvMass); // If other is not static
				////otherPos -= normal * penetration * (invMassBox / totalInvMass);

				//// Velocity
				//Vector3 relativeVelocity = mVelocity - otherObject.GetVelocity(); // Relative velocity:

				//float velAlongNormal = Dot(relativeVelocity, normal); // Velocity along the collision normal
				//if (velAlongNormal > 0.0f) // If they're already separating:
				//{
				//	return;
				//}

				//float e = 0.5f; // 0 = no bounce, 1 = perfect bounce // TODO: Adjust 

				//// Impulse magnitude:
				//float j = -(1.0f + e) * velAlongNormal / (invMassSphere + otherObject.GetMass());

				//// Impulse vector:
				//Vector3 impulse = j * normal;

				//////Apply:
				////sphereVelocity += impulse * invMassSphere;
				////boxVelocity -= impulse * invMassBox;

				//mVelocity += impulse * invMassSphere;
			}
		}
	}

	// Static collision
	//for (RBPhysicsObject& primaryObject : mDynamicObjects)
	//{
	//	for (RBPhysicsObject& staticObject : mStaticObjects)
	//	{
	//		IntersectData intersectData = primaryObject.GetCollider().Intersect(staticObject.GetCollider());
	//		if (intersectData.GetDoesIntersect())
	//		{
	//			//primaryObject.ResolveCollision(staticObject, intersectData);

	//			const Vector3 normal = intersectData.GetNormal();
	//			primaryObject.SetPosition(primaryObject.GetPosition() + normal * intersectData.GetPenetration());

	//			// If they both can move - Heavier objects move less
	//			const float invMass = 1.0f / primaryObject.GetMass();

	//			// Velocity
	//			const float velAlongNormal = Dot(primaryObject.GetVelocity(), normal); // Velocity along the collision normal
	//			if (velAlongNormal > 0.0f) // If they're already separating:
	//			{
	//				continue;
	//			}

	//			// Impulse magnitude:
	//			const float ImpulseMagnitude = -(1.0f + mSettings.bounceCoeficient) * velAlongNormal / invMass;

	//			// Impulse vector:
	//			const Vector3 impulse = ImpulseMagnitude * normal;

	//			//Apply:
	//			primaryObject.SetVelocity(primaryObject.GetVelocity() + impulse * invMass);
	//		}
	//	}
	//}

	for (RBPhysicsObject& primaryObject : mDynamicObjects)
	{
		for (RBPhysicsObject& staticObject : mStaticObjects)
		{
			IntersectData intersectData = primaryObject.GetCollider().Intersect(staticObject.GetCollider());
			if (intersectData.GetDoesIntersect())
			{
				// 1. Positional correction
				const Vector3 newPos = primaryObject.GetPosition() + intersectData.GetNormal() * (intersectData.GetPenetration()); // * 0.8f); = Baumgarte stabilization
				//const Vector3 newPos = primaryObject.GetPosition() + intersectData.GetNormal() * (intersectData.GetPenetration() * 1.1f);
				primaryObject.SetPosition(newPos);

				// 2. Velocity response
				Vector3 velocity = primaryObject.GetVelocity();
				float vn = Dot(velocity, intersectData.GetNormal());

				if (vn < 0.0f)
				{
					//Vector3 newVelocity = velocity - (1.0f + mSettings.bounceCoeficient) * vn * intersectData.GetNormal();
					//primaryObject.SetVelocity(newVelocity);
					Vector3 oppForce = vn * -intersectData.GetNormal();
					primaryObject.ApplyForce(oppForce);
				}
			}
		}
	}
}

void RBPhysicsWorld::DetectCollisionWithDome(float deltaTime)
{
	const float domeRadius = 8.0f;
	const Vector3 domeCenter = Vector3(0.0f, domeRadius, 0.0f);

	for (RBPhysicsObject& object : mDynamicObjects)
	{
		// Get ball radius from its collider (uses existing BoundingSphere)
		auto& collider = (BoundingSphere&)object.GetCollider();
		const float ballRadius = collider.GetRadius();

		const float centerDistance = Magnitude(object.GetPosition() - domeCenter);
		// If the ball center plus radius is outside the dome radius -> collision with inner surface
		if (centerDistance + ballRadius > domeRadius)
		{
			ResolveCollisionWithDome(object, deltaTime);
		}
	}
}

void RBPhysicsWorld::ResolveCollisionWithDome(RBPhysicsObject& object, float /*deltaTime*/)
{
	//const Vector3 domeCenter = Vector3(0.0f, 8.0f, 0.0f);
	//const float domeRadius = 8.0f;

	//// Ball radius from collider
	//auto& collider = (BoundingSphere&)object.GetCollider();
	//const float ballRadius = collider.GetRadius();

	//Vector3 relativePos = object.GetPosition() - domeCenter;
	//const float dist = Magnitude(relativePos);

	//// Guard: if degenerate, skip
	//if (dist <= 0.0f)
	//	return;

	//// Normal pointing from dome center to ball (outward)
	//Vector3 normal = Normalize(relativePos);

	//// penetration amount (positive if ball is outside the dome surface)
	//float penetration = (dist + ballRadius) - domeRadius;
	//if (penetration <= 0.0f)
	//	return;

	//// Positional correction: move ball back just inside the dome (small slop to avoid jitter)
	//const float k_slop = 0.001f;
	//const float correction = penetration + k_slop;
	//object.SetPosition(object.GetPosition() - normal * correction);

	//// Contact point in world space (on the sphere surface)
	//Vector3 contactPointWorld = object.GetPosition() - normal * ballRadius;
	//// Convert to local point for ApplyForceAtPoint (expects local point)
	//Vector3 localContactPoint = object.GetLocalPosition(contactPointWorld);

	//// Relative velocity at contact
	//Vector3 relativeVel = GetVelocityAtPoint(object, localContactPoint);
	//Vector3 velNormal = normal * Dot(relativeVel, normal);
	//Vector3 velTangent = relativeVel - velNormal;

	//// --- Normal response (velocity correction) ---
	//const float restitution = 0.3f; // bounce, tune as needed
	//// If there is outward normal velocity, reflect it (inelastic)
	//float vNormalScalar = Dot(relativeVel, normal);
	//if (vNormalScalar > 0.0f)
	//{
	//	// Remove outgoing normal component with restitution
	//	Vector3 v = object.GetVelocity();
	//	v -= velNormal * (1.0f + restitution); // reflect outward component
	//	object.SetVelocity(v);
	//}

	//// --- Tangential friction handled as a force at contact (produces torque) ---
	//// Estimate a tangential damping force (N) that tends to remove sliding
	//float effectiveMass = object.GetMass();
	//float tangentialDampening = object.GetTangentialDampening();
	//Vector3 tangentialForce = -velTangent * effectiveMass * tangentialDampening;

	//// Normal contact force magnitude estimate (simple)
	//float normalStiffness = object.GetNormalStiffness();
	//float normalDampening = object.GetNormalDampening();
	//Vector3 normalVel = velNormal;
	//Vector3 normalDisplacement = normal * penetration;
	//Vector3 normalForce = normalDisplacement * normalStiffness - normalVel * effectiveMass * normalDampening;

	//// Coulomb friction clamp
	//float mu = 0.5f;
	//float maxTangential = mu * Magnitude(normalForce);
	//if (Magnitude(tangentialForce) > maxTangential && maxTangential > 0.0f)
	//{
	//	tangentialForce = Normalize(tangentialForce) * maxTangential;
	//}

	//// Apply normal support (linear)
	//object.ApplyForce(normalForce);

	//// Apply tangential friction at contact point to create torque / rolling
	//object.ApplyForceAtPoint(tangentialForce, localContactPoint);

	//// Remove any outward acceleration component to avoid immediate re-penetration
	//Vector3 acc = object.GetAcceleration();
	//float accOut = Dot(acc, normal);
	//if (accOut > 0.0f)
	//{
	//	acc -= normal * accOut;
	//	object.SetAcceleration(acc);
	//}
}

Vector3 RBPhysicsWorld::GetVelocityAtPoint(const RBPhysicsObject& object, const Math::Vector3& localPoint)
{
	return object.GetVelocity() + object.GetOrientation() * Cross(object.GetAngularVelocity(), localPoint);
}

void RBPhysicsWorld::ResolveCollision(RBPhysicsObject& object1, RBPhysicsObject& object2, IntersectData& intersectData)
{

}