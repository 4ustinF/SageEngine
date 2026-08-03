#include "Precompiled.h"
#include "RigidBodyComponent.h"

#include "GameObject.h"
#include "GameWorld.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(RigidBodyComponent, 100);

void RigidBodyComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Rigid Body Component##RigidBodyComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		float mass = mMass;
		if (ImGui::DragFloat("Mass##RigidBodyComponent", &mass, 0.1f, 0.0f, 1000.0f))
		{
			SetMass(mass);
		}

		float drag = mDrag;
		if (ImGui::DragFloat("Drag##RigidBodyComponent", &drag, 0.1f, 0.0f, 1000.0f))
		{
			SetDrag(drag);
		}

		float angularDrag = mAngularDrag;
		if (ImGui::DragFloat("Angular Drag##RigidBodyComponent", &angularDrag, 0.1f, 0.0f, 1000.0f))
		{
			SetAngularDrag(angularDrag);
		}

		bool useGravity = mUseGravity;
		if (ImGui::Checkbox("Use Gravity##RigidBodyComponent", &useGravity))
		{
			SetUseGravity(useGravity);
		}

		bool isKinematic = mIsKinematic;
		if (ImGui::Checkbox("Is Kinematic##RigidBodyComponent", &isKinematic))
		{
			SetIsKinematic(isKinematic);
		}

		ImGui::Separator();

		ImGui::Text("Constraints");
		ImGui::Text("Freeze Position");
		ImGui::SameLine();
		if (ImGui::Checkbox("X##RBCompFreezePos", &mFreezePositionX))
		{
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Y##RBCompFreezePos", &mFreezePositionY))
		{
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Z##RBCompFreezePos", &mFreezePositionZ))
		{
		}

		ImGui::Text("Freeze Rotation");
		ImGui::SameLine();
		if (ImGui::Checkbox("X##RBCompFreezeRot", &mFreezeRotationX))
		{
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Y##RBCompFreezeRot", &mFreezeRotationY))
		{
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Z##RBCompFreezeRot", &mFreezeRotationZ))
		{
		}
	}
}

void RigidBodyComponent::OnEnable()
{
}

void RigidBodyComponent::OnDisable()
{
}

bool RigidBodyComponent::IsKinematic() const
{
	// return mMass > 0.0f; Is Dynamic check. Might be useful elsewhere?
	return mIsKinematic;
}

void RigidBodyComponent::SetMass(float mass)
{
	mMass = mass;
}

void RigidBodyComponent::SetDrag(float drag)
{
	mDrag = Max(0.0f, drag);
}

void RigidBodyComponent::SetAngularDrag(float angularDrag)
{
	mAngularDrag = Max(0.0f, angularDrag);
}

void RigidBodyComponent::SetUseGravity(bool useGravity)
{
	mUseGravity = useGravity;
}

void RigidBodyComponent::SetIsKinematic(bool isKinematic)
{
	mIsKinematic = isKinematic;
}

void RigidBodyComponent::SetConstraints(const Vector3Int& pos, const Vector3Int& rot)
{
	mFreezePositionX = pos.x == 1;
	mFreezePositionY = pos.y == 1;
	mFreezePositionZ = pos.z == 1;
	mFreezeRotationX = rot.x == 1;
	mFreezeRotationY = rot.y == 1;
	mFreezeRotationZ = rot.z == 1;

	if (mFreezePositionX || mFreezePositionY || mFreezePositionZ || mFreezeRotationX || mFreezeRotationY || mFreezeRotationZ)
	{
		// UpdateConstraints();
	}
}
