#include "SAGE/Inc/Precompiled.h"
#include "DoorITVComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(DoorITVComponent, 50);

void DoorITVComponent::Initialize()
{
	InteractTriggerVolumeComponent::Initialize();
}

void DoorITVComponent::Terminate()
{
	mDoorTransformComp = nullptr;
	mDoorGameObj = nullptr;
	InteractTriggerVolumeComponent::Terminate();
}

void DoorITVComponent::OnQueueUpdate(float deltaTime)
{
	mElpasedTime += deltaTime;
	const float percent = Clamp(mElpasedTime / mAnimationTime, 0.0f, 1.0f);
	Vector3 locPos = mDoorTransformComp->GetLocalPosition();
	locPos.z = mIsOpening ? Lerp(0.0f, mDoorEndPos, percent) : locPos.z = Lerp(mDoorEndPos, 0.0f, percent);
	mDoorTransformComp->SetLocalPosition(locPos);

	if (mElpasedTime < mAnimationTime)
	{
		EnqueueUpdate();
		return;
	}

	if (mIsOpening)
	{
		mDoorGameObj->SetActive(false);
	}

	mIsAnimating = false;
}

void DoorITVComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Door ITV Component##DoorITVComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{

	}
}

void DoorITVComponent::OnInteract()
{
	if (mIsAnimating)
	{
		return;
	}

	if (TransformComponent* doorTransformComp = GetDoorTransformComp())
	{
		mIsOpening = !mIsOpening;
		mElpasedTime = 0.0f;
		mIsAnimating = true;

		if (!mIsOpening)
		{
			mDoorGameObj->SetActive(true);
		}

		EnqueueUpdate();
	}
}

TransformComponent* DoorITVComponent::GetDoorTransformComp()
{
	if (mDoorTransformComp != nullptr)
	{
		return mDoorTransformComp;
	}

	if (mDoorGameObj != nullptr)
	{
		mDoorTransformComp = mDoorGameObj->GetComponent<TransformComponent>();
	}

	if (mDoorGameObj = GetOwner().FindChildByName("Door"))
	{
		mDoorTransformComp = mDoorGameObj->GetComponent<TransformComponent>();
	}

	return mDoorTransformComp;
}