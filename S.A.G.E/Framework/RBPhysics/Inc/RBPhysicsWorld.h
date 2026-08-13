#pragma once

#include "RBPhysicsObject.h"

namespace SAGE::RBPhysics
{
	class RBPhysicsObject;
	class Collider;
	class IntersectData;
	struct PhysicsRay;
	struct PhysicsRayHit;

	enum class PhysicsObjectType
	{
		Dynamic,
		Kinematic,
		Static
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
		};

		RBPhysicsWorld() = default;

		void Initialize(Settings settings = {});
		void Clear();

		void Update(float deltaTime);
		void DrawPhysicsObjects(bool fillShapes);
		//void DebugUI();

		RBPhysicsObject* AddObject(const RBPhysicsObject& object, PhysicsObjectType type = PhysicsObjectType::Static);
		bool RemoveObject(const RBPhysicsObject& object);

		// TODO: These are temp please remove.
		RBPhysicsObject& GetPhysicsObject(int index)
		{
			return mDynamicObjects[index];
		}

		// TODO: These are temp please remove.
		int GetObjectsCount() const
		{
			return static_cast<int>(mDynamicObjects.size());
		}

		// TODO: These are temp please remove.
		RBPhysicsObject& GetStaticPhysicsObject(int index)
		{
			return mStaticObjects[index];
		}

		// TODO: These are temp please remove.
		int GetStaticObjectsCount() const
		{
			return static_cast<int>(mStaticObjects.size());
		}

		bool Raycast(const Math::Vector3& origin, const Math::Vector3& direction, float maxDistance);
		bool Raycast(const PhysicsRay& ray);
		bool Raycast(const Math::Vector3& origin, const Math::Vector3& direction, float maxDistance, PhysicsRayHit& rayHit);
		bool Raycast(const PhysicsRay& ray, PhysicsRayHit& rayHit);

	private:
		void Simulate(float deltaTime);
		void HandleCollisions();

		Settings mSettings;

		// TODO: Convert to map of PhysicsObjectType
		std::vector<RBPhysicsObject> mDynamicObjects;	// Physics can affect the object.
		//std::vector<RBPhysicsObject> mKinematicObjects; // Physics cannot affect the object. Like static but can move.
		std::vector<RBPhysicsObject> mStaticObjects;	// Can't move.

		void DetectCollisionWithDome(float deltaTime);
		void ResolveCollisionWithDome(RBPhysicsObject& object, float deltaTime);
		Math::Vector3 GetVelocityAtPoint(const RBPhysicsObject& object, const Math::Vector3& localPoint);

		void ResolveCollision(RBPhysicsObject& object1, RBPhysicsObject& object2, IntersectData& intersectData);

		bool RaycastAgainstCollider(const PhysicsRay& ray, const Collider& collider, PhysicsRayHit& rayHit);
		bool RaycastAgainstBoundingBox(const PhysicsRay& ray, const BoundingBox& boundingBox, PhysicsRayHit& rayHit);
	};
}