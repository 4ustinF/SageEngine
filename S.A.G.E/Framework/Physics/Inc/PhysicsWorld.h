#pragma once

#include "Constraints.h"
#include "Particle.h"
#include "RigidAABB.h"

namespace SAGE::Physics
{
	struct Tetrahedron
	{
		Particle *a, *b, *c, *d;

		void SetPosition(const Math::Vector3& pos);
		void SetVelocity(const Math::Vector3& velocity);

		Math::Vector3 GetLook() const;
		Math::Vector3 GetRight() const;
		Math::Vector3 GetUp() const;
		Math::Vector3 GetPosition() const;
		
		Math::Matrix4 GetWorldMatrix() const;
	};

	class PhysicsWorld
	{
	public:
		struct Settings
		{
			Math::Vector3 gravity{ 0.0f, -9.81f, 0.0f };
			float timeStep = 1.0f / 60.0f;
			float drag = 0.0f;
			int iterations = 1;
		};

		void Initialize(Settings settings = {});

		void Update(float deltaTime);
		void DebugDraw() const;
		void DebugUI();

		Settings& GetSettings() { return mSettings; }

		void Pause(bool isPaused) { mPause = isPaused; }
		void ShowDebugLines(bool showDebug) { mShowDebugLines = showDebug; }
		bool IsPaused() { return mPause; }

		// For simulation
		template <class... Args>
		Particle* AddParticle(Args&&... args) {
			return mParticles.emplace_back(std::make_unique<Particle>(std::forward<Args>(args)...)).get();
		}

		template <class... Args>
		RigidAABB* AddRAABBs(Args&&... args) {
			return mRigidAABBs.emplace_back(std::make_unique<RigidAABB>(std::forward<Args>(args)...)).get();
		}

		template <class ConstraintType, class... Args> // Variadic template arguments
		ConstraintType* AddConstraint(Args&&... args) // Forwarding reference
		{
			auto& newConstraint = mConstraints.emplace_back(std::make_unique<ConstraintType>(std::forward<Args>(args)...)); // Perfect forwarding
			return static_cast<ConstraintType*>(newConstraint.get());
		}


		// For Eniviroment
		Tetrahedron AddTetrahedron();
		void AddPlane(const Math::Plane& plane);
		void AddOBB(const Math::OBB& obb);
		void AddAABB(const Math::AABB& aabb);

		const std::vector<Math::AABB>& GetAABBs() const { return mAABBs; }

		void Clear(bool dynamicOnly = false);
		void ClearAABBs() { mAABBs.clear(); };
		void ClearOBBs() { mOBBs.clear(); };
		void ClearPlanes() { mPlanes.clear(); };

	private:
		void AccumulateForces();
		void Intergrate();
		void SatisfyConstraints();

		std::vector<std::unique_ptr<Constraints>> mConstraints;
		std::vector<std::unique_ptr<Particle>> mParticles;
		std::vector<std::unique_ptr<RigidAABB>> mRigidAABBs;

		std::vector<Math::Plane> mPlanes;
		std::vector<Math::OBB> mOBBs;
		std::vector<Math::AABB> mAABBs;

		Settings mSettings;
		float mTimer = 0.0f;
		bool mPause = false;
		bool mShowDebugLines = true;
		bool mFillDebugShapes = false;
	};
}