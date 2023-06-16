#include "Precompiled.h"
#include "PhysicsWorld.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Physics;

namespace
{
	// Tetrahedron constants
	constexpr Math::Vector3 oa{+0.0f, 0.8165f, +0.0000f};
	constexpr Math::Vector3 ob{-0.5f, 0.0000f, -0.2887f};
	constexpr Math::Vector3 oc{+0.5f, 0.0000f, -0.2887f};
	constexpr Math::Vector3 od{+0.0f, 0.0000f, +0.5774f};
}

void Tetrahedron::SetPosition(const Math::Vector3& pos)
{
	a->SetPosition(pos + oa);
	b->SetPosition(pos + ob);
	c->SetPosition(pos + oc);
	d->SetPosition(pos + od);
}

void Tetrahedron::SetVelocity(const Math::Vector3& velocity)
{
	a->SetVelocity(velocity + oa);
	b->SetVelocity(velocity + ob);
	c->SetVelocity(velocity + oc);
	d->SetVelocity(velocity + od);
}

Vector3 Tetrahedron::GetLook() const
{
	Math::Vector3 pos = GetPosition();
	return Math::Normalize(d->position - pos);
}

Vector3 Tetrahedron::GetRight() const
{
	return Math::Normalize(c->position - b->position);
}

Vector3 Tetrahedron::GetUp() const
{
	Math::Vector3 pos = GetPosition();
	return Math::Normalize(a->position - pos);
}

Vector3 Tetrahedron::GetPosition() const
{
	return (b->position + c->position + d->position) * 0.3333f;
}

Matrix4 Tetrahedron::GetWorldMatrix() const
{
	Math::Vector3 l = GetLook();
	Math::Vector3 r = GetRight();
	Math::Vector3 u = GetUp();
	Math::Vector3 p = GetPosition();
	return{
		r.x, r.y, r.z, 0.0f,
		u.x, u.y, u.z, 0.0f,
		l.x, l.y, l.z, 0.0f,
		p.x, p.y, p.z, 0.0f
	};
}

void PhysicsWorld::Initialize(Settings settings)
{
	mSettings = std::move(settings);
}

void PhysicsWorld::Update(float deltaTime)
{
	if (mPause) { return; }

	mTimer += deltaTime;
	while (mTimer >= mSettings.timeStep)
	{
		mTimer -= mSettings.timeStep;

		AccumulateForces();
		Intergrate();
		SatisfyConstraints();
	}
}

void PhysicsWorld::DebugDraw() const
{
	if (!mShowDebugLines) { return; }

	for (auto& c : mConstraints) {
		c->DebugDraw();
	}

	for (auto& p : mParticles) {
		SimpleDraw::AddSphere(p->position, 4, 2, p->radius, Colors::Cyan);
	}

	if (mFillDebugShapes) {
		for (auto& obb : mOBBs) { SimpleDraw::AddFilledOBB(obb, Colors::Blue); }
		for (auto& aabb : mAABBs) { SimpleDraw::AddFilledAABB(aabb, Colors::BlueViolet); }
		for (auto& rAABB : mRigidAABBs) { SimpleDraw::AddFilledAABB(rAABB->aabb, Colors::CadetBlue); }
	}
	else {
		for (auto& obb : mOBBs) { SimpleDraw::AddOBB(obb, Colors::Blue); }
		for (auto& aabb : mAABBs) { SimpleDraw::AddAABB(aabb, Colors::BlueViolet); }
		for (auto& rAABB : mRigidAABBs) { SimpleDraw::AddAABB(rAABB->aabb, Colors::CadetBlue); }
	}
}

void PhysicsWorld::DebugUI()
{
	ImGui::Begin("Physics", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat3("Gravity", &mSettings.gravity.x, 0.1f, -10.0f, 10.0f);
	ImGui::Checkbox("Pause simulation", &mPause);
	ImGui::Checkbox("Show Debug Lines", &mShowDebugLines);
	ImGui::Checkbox("Fill Debug Shapes", &mFillDebugShapes);
	ImGui::End();
}

Tetrahedron PhysicsWorld::AddTetrahedron()
{
	Tetrahedron tetrahedron;
	tetrahedron.a = AddParticle();
	tetrahedron.b = AddParticle();
	tetrahedron.c = AddParticle();
	tetrahedron.d = AddParticle();

	tetrahedron.a->position = oa;
	tetrahedron.b->position = ob;
	tetrahedron.c->position = oc;
	tetrahedron.d->position = od;

	tetrahedron.a->radius = 0.2f;
	tetrahedron.b->radius = 0.2f;
	tetrahedron.c->radius = 0.2f;
	tetrahedron.d->radius = 0.2f;

	tetrahedron.a->invMass = 1.0f;
	tetrahedron.b->invMass = 1.0f;
	tetrahedron.c->invMass = 1.0f;
	tetrahedron.d->invMass = 1.0f;

	AddConstraint<Spring>(tetrahedron.a, tetrahedron.b);
	AddConstraint<Spring>(tetrahedron.a, tetrahedron.c);
	AddConstraint<Spring>(tetrahedron.a, tetrahedron.d);

	AddConstraint<Spring>(tetrahedron.b, tetrahedron.c);
	AddConstraint<Spring>(tetrahedron.c, tetrahedron.d);
	AddConstraint<Spring>(tetrahedron.d, tetrahedron.b);

	return tetrahedron;
}

void PhysicsWorld::AddPlane(const Plane& plane)
{
	mPlanes.push_back(plane);
}

void PhysicsWorld::AddOBB(const OBB& obb)
{
	mOBBs.push_back(obb);
}

void PhysicsWorld::AddAABB(const AABB& aabb)
{
	mAABBs.push_back(aabb);
}

void PhysicsWorld::Clear(bool dynamicOnly)
{
	mConstraints.clear();
	mParticles.clear();

	if (!dynamicOnly)
	{
		mPlanes.clear();
		mOBBs.clear();
		mAABBs.clear();
		mRigidAABBs.clear();
	}
}


void PhysicsWorld::AccumulateForces()
{
	for (auto& p : mParticles) {
		p->acceleration = mSettings.gravity;
	}

	for (auto& rAABB : mRigidAABBs) {
		rAABB->acceleration = mSettings.gravity;
	}
}

void PhysicsWorld::Intergrate()
{
	const float timeStepSqr = Math::Sqr(mSettings.timeStep);
	for (auto& p : mParticles)
	{
		const auto newPosition = (p->position * 2.0f) - p->lastPosition + (p->acceleration * timeStepSqr);
		p->lastPosition = p->position;
		p->position = newPosition;
	}

	for (auto& rAABB : mRigidAABBs) {
		auto newPosition = (rAABB->aabb.center * 2.0f) - rAABB->lastPosition + ((rAABB->acceleration + rAABB->force) * timeStepSqr);
		rAABB->lastPosition = rAABB->aabb.center;
		rAABB->aabb.center = newPosition;
	}
}

void PhysicsWorld::SatisfyConstraints()
{
	for (int i = 0; i < mSettings.iterations; ++i)
	{
		for (auto& c : mConstraints) {
			c->Apply();
		}
	}

	for (auto& plane : mPlanes)
	{
		for (auto& p : mParticles)
		{
			float penetration = 0.0f;

			// Are we colliding right now?
			if (Intersect(Math::Sphere{ p->position , p->radius }, plane, penetration))
			{
				auto velocity = p->position - p->lastPosition;
				auto velocityPerpendicular = plane.normal * Math::Dot(velocity, plane.normal);
				auto velocityParallel = velocity - velocityPerpendicular;

				// Were we colliding?
				float temp = 0.0f;
				if (Math::Intersect(Math::Sphere{ p->lastPosition, p->radius }, plane, temp))
				{
					const auto newVelocity = velocityParallel * (1.0f - mSettings.drag);
					p->SetPosition(p->position + (plane.normal * penetration));
					p->SetVelocity(newVelocity);
				}
				else
				{
					const auto newVelocity = (velocityParallel * (1.0f - mSettings.drag)) - (velocityPerpendicular * p->bounce);
					p->SetPosition(p->position - velocityPerpendicular);
					p->SetVelocity(newVelocity);
				}
			}
		}

		for (auto& rAABB : mRigidAABBs) {
			float penetration = 0.0f;

			if (Intersect(rAABB->aabb, plane, penetration))
			{
				auto velocity = rAABB->GetVelocity();
				auto velocityPerpendicular = plane.normal * Dot(velocity, plane.normal);
				auto velocityParallel = velocity - velocityPerpendicular;

				const auto newVelocity = velocityParallel * (1.0f - mSettings.drag);
				rAABB->SetPosition(rAABB->aabb.center + (plane.normal * penetration));
				rAABB->SetVelocity(newVelocity);
			}
		}
	}

	for (auto& obb : mOBBs)
	{
		for (auto& p : mParticles)
		{
			if (Math::Intersect(p->position, obb))
			{
				auto velocity = p->position - p->lastPosition;
				auto direction = Math::Normalize(velocity);

				Math::Ray ray{ p->lastPosition, direction };
				Math::Vector3 point, normal;
				Math::Intersect(ray, obb, point, normal);

				auto velocityPerpendicular = normal * Math::Dot(velocity, normal);
				auto velocityParallel = velocity - velocityPerpendicular;
				auto newVelocity = (velocityParallel * (1.0f - mSettings.drag)) - (velocityPerpendicular * p->bounce);
				p->SetPosition(p->position - velocityPerpendicular);
				p->SetVelocity(newVelocity);
			}
		}
	}

	for (auto& aabb : mAABBs)
	{
		for (auto& p : mParticles)
		{
			if (Math::Intersect(p->position, aabb))
			{
				auto velocity = p->position - p->lastPosition;
				auto direction = Math::Normalize(velocity);

				Math::Ray ray{ p->lastPosition, direction };
				Math::Vector3 point, normal;
				Math::OBB obb = {aabb.center, aabb.extend, Math::Quaternion::Identity};
				Math::Intersect(ray, obb, point, normal);

				auto velocityPerpendicular = normal * Math::Dot(velocity, normal);
				auto velocityParallel = velocity - velocityPerpendicular;
				auto newVelocity = (velocityParallel * (1.0f - mSettings.drag)) - (velocityPerpendicular * p->bounce);
				p->SetPosition(p->position - velocityPerpendicular);
				p->SetVelocity(newVelocity);
			}
		}

		for (auto& rAABB : mRigidAABBs) {
			Vector3 normal = Vector3::One;
			float penetration = 0;

			if (Intersect(rAABB->aabb, aabb, normal, penetration))
			{
				auto velocity = rAABB->GetVelocity();
				auto velocityPerpendicular = normal * Dot(velocity, normal);
				auto velocityParallel = velocity - velocityPerpendicular;

				const auto newVelocity = velocityParallel * (1.0f - mSettings.drag);
				rAABB->SetPosition(rAABB->aabb.center + (normal * penetration));
				rAABB->SetVelocity(newVelocity);
			}
		}
	}
}