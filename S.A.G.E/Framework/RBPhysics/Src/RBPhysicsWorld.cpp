#include "Precompiled.h"
#include "RBPhysicsWorld.h"

#include "IntersectData.h"
#include "BoundingBox.h"
#include "BoundingCapsule.h"
#include "BoundingSphere.h"
#include "PhysicsRay.h"

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
}

void RBPhysicsWorld::DrawPhysicsObjects(bool fillShapes)
{
	for (std::unique_ptr<RBPhysicsObject>& object : mDynamicObjects)
	{
		object->DebugDraw(fillShapes);
	}

	for (std::unique_ptr<RBPhysicsObject>& object : mStaticObjects)
	{
		object->DebugDraw(fillShapes);
	}
}

void RBPhysicsWorld::DebugUI()
{
	if (mDynamicObjects.empty())
		return;

	ImGui::Begin("Physics", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat3("Gravity", &mSettings.gravity.x, 0.1f);
	ImGui::Separator();

	for (const auto& object : mDynamicObjects)
	{
		Vector3 pos = object->GetPosition();
		ImGui::DragFloat3("Object", &pos.x, 0.1f);
	}

	for (const auto& object : mStaticObjects)
	{
		Vector3 pos = object->GetPosition();
		ImGui::DragFloat3("Object", &pos.x, 0.1f);
	}

	ImGui::End();
}

RBPhysicsObject* RBPhysicsWorld::CreatePhysicsObject(std::unique_ptr<Collider> collider, PhysicsObjectType type /*= PhysicsObjectType::Static*/)
{
	auto obj = std::make_unique<RBPhysicsObject>(std::move(collider));
	RBPhysicsObject* rawPtr = obj.get();

	switch (type)
	{
	case PhysicsObjectType::Dynamic:
	case PhysicsObjectType::Kinematic: // TODO: 
		mDynamicObjects.push_back(std::move(obj)); // world owns it now
		return rawPtr; // non-owning pointer back to caller
		break;
	case PhysicsObjectType::Static:

		mStaticObjects.push_back(std::move(obj)); // world owns it now
		return rawPtr; // non-owning pointer back to caller
	}

	return nullptr;
}

bool RBPhysicsWorld::RemoveObject(const RBPhysicsObject* object)
{
	if (object == nullptr)
	{
		return false;
	}

	PurgeTriggerPairs(object);

	auto it = std::find_if(mStaticObjects.begin(), mStaticObjects.end(),
		[object](const std::unique_ptr<RBPhysicsObject>& obj) {
			return obj.get() == object;
		});

	if (it != mStaticObjects.end()) {
		mStaticObjects.erase(it); // unique_ptr goes out of scope here -> deletes the object
		return true;
	}

	it = std::find_if(mDynamicObjects.begin(), mDynamicObjects.end(),
		[object](const std::unique_ptr<RBPhysicsObject>& obj) {
			return obj.get() == object;
		});

	if (it != mDynamicObjects.end()) {
		mDynamicObjects.erase(it); // unique_ptr goes out of scope here -> deletes the object
		return true;
	}

	return false;
}

void RBPhysicsWorld::Simulate(float deltaTime)
{
	for (std::unique_ptr<RBPhysicsObject>& dynamicObject : mDynamicObjects)
	{
		// Gravity
		dynamicObject->ApplyForce(mSettings.gravity * deltaTime);

		// Air Drag
		Vector3 velocity = dynamicObject->GetVelocity();
		float speed = Magnitude(velocity);

		if (speed > 0.0001f)
		{
			// Simple linear drag: force opposes velocity, proportional to speed
			float dragCoefficient = 0.01f; // tune this
			Vector3 dragForce = -velocity * dragCoefficient;

			// Or quadratic drag (more realistic, stronger at high speed):
			// Vector3 dragForce = -Normalize(velocity) * (dragCoefficient * speed * speed);

			dragForce.y = 0.0f; // Optional: ignore vertical drag if desired
			dynamicObject->ApplyForce(dragForce);
		}

		dynamicObject->Integrate(deltaTime);
	}
}

void RBPhysicsWorld::HandleCollisions()
{
	const int objectsCount = static_cast<int>(mDynamicObjects.size());
	for (int primaryIndex = 0; primaryIndex < objectsCount; ++primaryIndex)
	{
		std::unique_ptr<RBPhysicsObject>& primaryObject = mDynamicObjects[primaryIndex];
		const bool isPrimaryObjectATrigger = primaryObject->GetIsTrigger();

		for (int secondaryIndex = primaryIndex + 1; secondaryIndex < objectsCount; ++secondaryIndex)
		{
			std::unique_ptr<RBPhysicsObject>& secondaryObject = mDynamicObjects[secondaryIndex];
			const bool isSecondaryObjectATrigger = secondaryObject->GetIsTrigger();
			if (isPrimaryObjectATrigger && isSecondaryObjectATrigger)
			{
				continue;
			}

			IntersectData intersectData = primaryObject->GetCollider()->Intersect(secondaryObject->GetCollider());
			if (intersectData.GetDoesIntersect())
			{
				if (isPrimaryObjectATrigger || isSecondaryObjectATrigger)
				{
					mCurrentTriggerPairs.emplace(primaryObject.get(), secondaryObject.get());
				}
				else
				{
					primaryObject->ResolveCollision(secondaryObject, intersectData);
					intersectData.InverseNormal(); // Inverse the normal vector for the second object
					secondaryObject->ResolveCollision(primaryObject, intersectData);
				}
			}
		}
	}

	for (std::unique_ptr<RBPhysicsObject>& primaryObject : mDynamicObjects)
	{
		const bool isPrimaryObjectATrigger = primaryObject->GetIsTrigger();
		for (std::unique_ptr<RBPhysicsObject>& staticObject : mStaticObjects)
		{
			const bool isSecondaryObjectATrigger = staticObject->GetIsTrigger();
			if (isPrimaryObjectATrigger && isSecondaryObjectATrigger)
			{
				continue;
			}

			IntersectData intersectData = primaryObject->GetCollider()->Intersect(staticObject->GetCollider());
			if (intersectData.GetDoesIntersect())
			{
				if (isPrimaryObjectATrigger || isSecondaryObjectATrigger)
				{
					mCurrentTriggerPairs.emplace(primaryObject.get(), staticObject.get());
				}
				else
				{
					primaryObject->ResolveCollision(intersectData);
				}
			}
		}
	}

	ProcessTriggerEvents();
}

void RBPhysicsWorld::ProcessTriggerEvents()
{
	// Enter: in current, not in previous
	for (const TriggerPairKey& pair : mCurrentTriggerPairs)
	{
		bool wasOverlappingLastFrame = mPreviousTriggerPairs.find(pair) != mPreviousTriggerPairs.end();

		if (!wasOverlappingLastFrame)
		{
			pair.first->BraodcastTriggerEnterDelegate(pair.second->GetCollider());
			pair.second->BraodcastTriggerEnterDelegate(pair.first->GetCollider());
		}
		else
		{
			pair.first->BraodcastTriggerStayDelegate(pair.second->GetCollider());
			pair.second->BraodcastTriggerStayDelegate(pair.first->GetCollider());
		}
	}

	// Exit: was in previous, not in current
	for (const TriggerPairKey& pair : mPreviousTriggerPairs)
	{
		if (mCurrentTriggerPairs.find(pair) == mCurrentTriggerPairs.end())
		{
			pair.first->BraodcastTriggerExitDelegate(pair.second->GetCollider());
			pair.second->BraodcastTriggerExitDelegate(pair.first->GetCollider());
		}
	}

	mPreviousTriggerPairs = mCurrentTriggerPairs; // This frame becomes "previous" for next frame
}

void RBPhysicsWorld::PurgeTriggerPairs(const RBPhysicsObject* object)
{
	auto purge = [object](std::unordered_set<TriggerPairKey, TriggerPairKeyHash>& set) {
		for (auto it = set.begin(); it != set.end();)
		{
			if (it->first == object || it->second == object)
				it = set.erase(it);
			else
				++it;
		}
	};

	purge(mCurrentTriggerPairs);
	purge(mPreviousTriggerPairs);
}

void RBPhysicsWorld::ResolveCollision(RBPhysicsObject& object1, RBPhysicsObject& object2, IntersectData& intersectData)
{
}

#pragma region ---RayCast---

bool RBPhysicsWorld::Raycast(const Math::Vector3& origin, const Math::Vector3& direction, float maxDistance)
{
	return Raycast(PhysicsRay(origin, direction, maxDistance));
}

bool RBPhysicsWorld::Raycast(const PhysicsRay& ray) // TODO: Instead should do a check that doesn't need to calculate hit data to save on perf?
{
	PhysicsRayHit hit;
	return Raycast(ray, hit);
}

bool RBPhysicsWorld::Raycast(const Math::Vector3& origin, const Math::Vector3& direction, float maxDistance, PhysicsRayHit& rayHit)
{
	return Raycast(PhysicsRay(origin, direction, maxDistance), rayHit);
}

bool RBPhysicsWorld::Raycast(const PhysicsRay& ray, PhysicsRayHit& rayHit)
{
	SimpleDraw::AddLine(ray.origin, ray.GetEndPoint(), Colors::Cyan);
	PhysicsRayHit newHit;

	for (std::unique_ptr<RBPhysicsObject>& staticObject : mStaticObjects)
	{
		if (RaycastAgainstCollider(ray, staticObject->GetCollider(), newHit))
		{
			if (newHit.distance < rayHit.distance)
			{
				rayHit = newHit;
			}
		}
	}

	return rayHit.hit; // TODO: Return the closest hit information instead of just a bool. Maybe out with the hit info.
}

bool RBPhysicsWorld::RaycastAgainstCollider(const PhysicsRay& ray, const Collider* collider, PhysicsRayHit& rayHit) // TODO: Can we just remove this casting into Raycast()
{
	if (collider == nullptr)
	{
		return false;
	}

	switch (collider->GetType()) // TODO: Other cases.
	{
	case Collider::ColliderType::TYPE_BOX:
		return RaycastAgainstBoundingBox(ray, dynamic_cast<const BoundingBox*>(collider), rayHit);
	case Collider::ColliderType::TYPE_CAPSULE:
		return RaycastAgainstBoundingCapsule(ray, dynamic_cast<const BoundingCapsule*>(collider), rayHit);
	case Collider::ColliderType::TYPE_SPHERE:
		return RaycastAgainstBoundingSphere(ray, dynamic_cast<const BoundingSphere*>(collider), rayHit);
	}

	return false;
}

bool RBPhysicsWorld::RaycastAgainstBoundingBox(const PhysicsRay& ray, const BoundingBox* boundingBox, PhysicsRayHit& rayHit)
{
	// Compute the local to world / world to local matrices
	const Matrix4 matTrans = Matrix4::Translation(boundingBox->GetCenter());
	const Matrix4 matRot = Matrix4::RotationQuaternion(boundingBox->GetOrientation());
	const Matrix4 matWorld = matRot * matTrans;
	const Matrix4 matWorldInv = Inverse(matWorld);

	// Transform the ray into the OBB's local space -- now it's just
	// a ray vs. an axis-aligned box centered at the origin with
	// half-extents obb.extend.
	const Vector3 org = TransformCoord(ray.origin, matWorldInv);
	const Vector3 dir = TransformNormal(ray.direction, matWorldInv);

	const float origin[3] = { org.x, org.y, org.z };
	const float direction[3] = { dir.x, dir.y, dir.z };
	const Vector3& extend = boundingBox->GetExtend();
	const float extent[3] = { extend.x, extend.y, extend.z };

	float tMin = 0.0f;
	float tMax = ray.maxDistance;
	int hitAxis = -1;
	float hitSign = 0.0f;

	for (int axis = 0; axis < 3; ++axis)
	{
		if (fabs(direction[axis]) < Constants::Epsilon)
		{
			// Ray is parallel to this pair of slabs -- it only misses if the origin isn't between them.
			if (origin[axis] < -extent[axis] || origin[axis] > extent[axis])
			{
				return false;
			}
			continue;
		}

		const float invD = 1.0f / direction[axis];
		float t1 = (-extent[axis] - origin[axis]) * invD;
		float t2 = (extent[axis] - origin[axis]) * invD;
		float sign = -1.0f;

		if (t1 > t2)
		{
			std::swap(t1, t2);
			sign = 1.0f;
		}

		if (t1 > tMin)
		{
			tMin = t1;
			hitAxis = axis;
			hitSign = sign;
		}

		tMax = std::min(tMax, t2);

		if (tMin > tMax)
		{
			return false; // slabs don't overlap -- no hit
		}
	}

	rayHit.hit = true;
	rayHit.distance = tMin;
	rayHit.impactPoint = ray.origin + ray.direction * tMin;

	// hitAxis stays -1 only when the ray origin started inside the box
	// (tMin was never pushed forward by an entry plane). In that case
	// there's no meaningful "entry" normal -- position is just ray.origin.
	if (hitAxis != -1)
	{
		Vector3 localNormal{ 0.0f, 0.0f, 0.0f };
		if (hitAxis == 0) localNormal.x = hitSign;
		else if (hitAxis == 1) localNormal.y = hitSign;
		else localNormal.z = hitSign;

		// matWorld is rotation + translation only (no scale), so it's
		// safe to transform the normal directly -- no inverse-transpose needed.
		rayHit.normal = TransformNormal(localNormal, matWorld);
	}

	return true;
}

bool RBPhysicsWorld::RaycastAgainstBoundingCapsule(const PhysicsRay& ray, const BoundingCapsule* boundingCapsule, PhysicsRayHit& rayHit)
{
	return false; // TODO:
}

bool RBPhysicsWorld::RaycastAgainstBoundingSphere(const PhysicsRay& ray, const BoundingSphere* boundingSphere, PhysicsRayHit& rayHit)
{
	return false; // TODO:
}

#pragma endregion