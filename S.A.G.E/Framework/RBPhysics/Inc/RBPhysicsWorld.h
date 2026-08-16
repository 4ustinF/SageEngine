#pragma once

#include "RBPhysicsObject.h"

namespace SAGE::RBPhysics
{
	class RBPhysicsObject;
	class Collider;
	class BoundingBox;
	class BoundingCapsule;
	class BoundingSphere;
	class IntersectData;
	struct PhysicsRay;
	struct PhysicsRayHit;

	enum class PhysicsObjectType
	{
		Dynamic,
		Kinematic,
		Static
	};

	struct TriggerPairKey
	{
		RBPhysicsObject* first;
		RBPhysicsObject* second;

		TriggerPairKey(RBPhysicsObject* a, RBPhysicsObject* b)
		{
			// Normalize order so (A,B) and (B,A) hash/compare the same
			if (a < b) { first = a; second = b; }
			else { first = b; second = a; }
		}

		bool operator==(const TriggerPairKey& other) const
		{
			return first == other.first && second == other.second;
		}
	};

	struct TriggerPairKeyHash
	{
		size_t operator()(const TriggerPairKey& key) const
		{
			size_t h1 = std::hash<RBPhysicsObject*>()(key.first);
			size_t h2 = std::hash<RBPhysicsObject*>()(key.second);
			return h1 ^ (h2 << 1);
		}
	};

	class RBPhysicsWorld
	{
	public:
		struct Settings
		{
			Math::Vector3 gravity{ 0.0f, -9.81f, 0.0f };
			float drag = 0.0f;
			float maxAirdrag = 1.0f;
			float airDragCoeficient = 0.05f;
			float bounceCoeficient = 0.0f; // 0 = no bounce, 1 = perfect bounce = restitution
			float maxSlopeDot = 0.7071f; // 0.7071 ~= cos(45 degrees)
		};

		RBPhysicsWorld() = default;

		void Initialize(Settings settings = {});
		void Clear();

		void Update(float deltaTime);
		void DrawPhysicsObjects(bool fillShapes);
		void DebugUI();

		//RBPhysicsObject* AddObject(RBPhysicsObject& object, PhysicsObjectType type = PhysicsObjectType::Static);
		RBPhysicsObject* CreatePhysicsObject(std::unique_ptr<Collider> collider, PhysicsObjectType type = PhysicsObjectType::Static);
		bool RemoveObject(const RBPhysicsObject* object);

		bool Raycast(const Math::Vector3& origin, const Math::Vector3& direction, float maxDistance);
		bool Raycast(const PhysicsRay& ray);
		bool Raycast(const Math::Vector3& origin, const Math::Vector3& direction, float maxDistance, PhysicsRayHit& rayHit);
		bool Raycast(const PhysicsRay& ray, PhysicsRayHit& rayHit);

	private:
		void Simulate(float deltaTime);
		void HandleCollisions();
		void ProcessTriggerEvents();
		void PurgeTriggerPairs(const RBPhysicsObject* object);

		void ResolveCollision(RBPhysicsObject& object1, RBPhysicsObject& object2, IntersectData& intersectData);
		bool RaycastAgainstCollider(const PhysicsRay& ray, const Collider* collider, PhysicsRayHit& rayHit);
		bool RaycastAgainstBoundingBox(const PhysicsRay& ray, const BoundingBox* boundingBox, PhysicsRayHit& rayHit);
		bool RaycastAgainstBoundingCapsule(const PhysicsRay& ray, const BoundingCapsule* boundingCapsule, PhysicsRayHit& rayHit);
		bool RaycastAgainstBoundingSphere(const PhysicsRay& ray, const BoundingSphere* boundingSphere, PhysicsRayHit& rayHit);

		Settings mSettings;

		// TODO: Convert to map of PhysicsObjectType
		std::vector<std::unique_ptr<RBPhysicsObject>> mDynamicObjects;	// Physics can affect the object.
		std::vector<std::unique_ptr<RBPhysicsObject>> mStaticObjects;	// Can't move.

		std::unordered_set<TriggerPairKey, TriggerPairKeyHash> mCurrentTriggerPairs;
		std::unordered_set<TriggerPairKey, TriggerPairKeyHash> mPreviousTriggerPairs;
	};
}