#include "SAGE/Inc/Precompiled.h"
#include "BulletHolesPoolComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(BulletHolesPoolComponent, 1);

void BulletHolesPoolComponent::LoadComponentFromTemplate(const rj::Value& value)
{

}

void BulletHolesPoolComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{

}

void BulletHolesPoolComponent::Initialize()
{

}

void BulletHolesPoolComponent::Terminate()
{
	mActiveBulletHoles.empty();
	mDisabledBulletHoles.empty();
}

void BulletHolesPoolComponent::OnQueueUpdate(float deltaTime)
{
	for (int activeIndex = static_cast<int>(mActiveBulletHoles.size()) - 1; activeIndex >= 0; --activeIndex)
	{
		BulletHole& bulletHole = mActiveBulletHoles[activeIndex];

		bulletHole.remainingLifeTime -= deltaTime;
		if (bulletHole.remainingLifeTime <= 0.0f)
		{
			bulletHole.gameObject->SetActive(false);
			mDisabledBulletHoles.push_back(std::move(mActiveBulletHoles[activeIndex]));
			mActiveBulletHoles.erase(mActiveBulletHoles.begin() + activeIndex);
		}

		// TODO: Fade emmisiveness
	}

	if (mActiveBulletHoles.size() > 0)
	{
		EnqueueUpdate();
	}
}

void BulletHolesPoolComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Bullet Holes Pool Component##BulletHolesPoolComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		// TODO: Count?
	}
}

void BulletHolesPoolComponent::SpawnBulletHole(const SAGE::Math::Vector3& position, const SAGE::Math::Quaternion& rotation, float scale, float lifeTime)
{
	if (mDisabledBulletHoles.size() > 0)
	{
		BulletHole& bulletHole = mDisabledBulletHoles.back();
		bulletHole.transformComp->SetPosition(position);
		bulletHole.transformComp->SetRotation(rotation);
		bulletHole.transformComp->SetScale(scale);
		bulletHole.remainingLifeTime = lifeTime;

		mActiveBulletHoles.push_back(std::move(bulletHole));
		mDisabledBulletHoles.pop_back();

		bulletHole.gameObject->SetActive(true);
	}
	else
	{
		GameObject& owner = GetOwner();
		GameObject* bulletHoleGO = owner.GetWorld().CreateGameObject("../../Assets/Templates/HalfLifeTemplates/Player/Weapons/BulletHoles/bullet_hole.json", &owner);

		BulletHole bulletHole;
		bulletHole.gameObject = bulletHoleGO;
		bulletHole.transformComp = bulletHoleGO->GetComponent<TransformComponent>();
		bulletHole.transformComp->SetPosition(position);
		bulletHole.transformComp->SetRotation(rotation);
		bulletHole.transformComp->SetScale(scale);
		bulletHole.remainingLifeTime = lifeTime;
		mActiveBulletHoles.push_back(std::move(bulletHole));
	}

	EnqueueUpdate();
}
