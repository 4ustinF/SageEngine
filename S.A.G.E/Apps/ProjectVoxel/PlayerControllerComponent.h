#pragma once

#include "TypeIds.h"
#include "InventorySlot.h"

class WorldService;
class SAGE::CameraService;
class PlayerUIService;

class PlayerControllerComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::PlayerController);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void DebugUI() override;
	void OnEnable() override;

	bool IsSurvivalMode() { return mSurvivalMode; }
	bool IsMining() { return mIsMining && !mMineInstantBlock; }

	void TakeDamage(int dmg);
	void GainHealth(int hp);

	void AddItemToInventory(ItemTypes itemType);

	SAGE::Graphics::RenderObject mMiningCube;

private:
	void MovePlayer(float deltaTime);
	void SurvivalMovePlayer(float deltaTime, SAGE::Math::Vector3& forward, const SAGE::Math::Vector3& right);
	void CreativeMovePlayer(float deltaTime, SAGE::Math::Vector3& forward, const SAGE::Math::Vector3& right);
	bool GroundCheck();
	void UpdateFalling(bool isFalling);
	void DeceleratePlayer();

	void BlockHover(const SAGE::Graphics::Camera& camera);
	void PlayerLeftClick(SAGE::Math::Vector3 closestPoint, SAGE::Math::Vector3 closestNormal);
	void PlayerMiddleClick(SAGE::Math::Vector3 closestPoint, SAGE::Math::Vector3 closestNormal);
	void PlayerRightClick(SAGE::Math::Vector3 closestPoint, SAGE::Math::Vector3 closestNormal);
	bool InteractWithBlock(SAGE::Math::Vector3 closestPoint, SAGE::Math::Vector3 closestNormal);

	void Hoeing(SAGE::Math::Vector3 closestPoint, SAGE::Math::Vector3 closestNormal);

	SAGE::Math::Vector2Int GetClosestPointCoords(SAGE::Math::Vector3& closestPoint, SAGE::Math::Vector3 closestNormal, bool isLeftClick = true);

	ItemTypes BlockTypeToItemType(const BlockTypes blockType);
	BlockTypes ItemTypeToBlockType(const ItemTypes itemType);

	void PlayBlockBreakingSound(const BlockTypes blockType);
	void PlayBlockPlacingSound(const BlockTypes blockType);

	btRigidBody* mPlayerRB = nullptr;
	SAGE::Math::Vector3 mLinearVelocity = SAGE::Math::Vector3::Zero;
	float mWalkSpeed = 50.0f;
	float mSprintSpeed = 80.0f;
	float mMaxSpeed = 9.0f;
	float mAirSpeed = 65.0f;
	float mJumpSpeed = 20.0f; 
	float mGravity = -25.0f;

	// SoundIds
	void InitializeSoundIds();
	SAGE::Graphics::SoundId mCrunchID;
	SAGE::Graphics::SoundId mGlassBreakID;
	SAGE::Graphics::SoundId mLeafBreakID;
	SAGE::Graphics::SoundId mHoeID;

	InventorySlot mInventory[36];
	//InventorySlot mArmorSlots[4];
	bool mSurvivalMode = true;

	SAGE::Graphics::Color GetBlockColor(const BlockTypes blockType);

	// Health
	void Respawn();
	void PassiveHeal(float deltaTime);
	int mHealth = 20;
	const int mMaxHealth = 20;
	float mHealthTickTimer = 0.0f;
	const float mHealthTickStep = 4.0f;

	// Hunger
	void PassiveHunger(float deltaTime);
	void EatFood(ItemTypes foodItem);
	int mHunger = 20;
	const int mHungerThreshold = 15;
	int mMaxHunger = 20;
	float mHungerTickTimer = 0.0f;
	const float mHungerTickStep = 10.0f;

	// Falling
	bool mIsFalling = false;
	float mStartFallYValue = 0;

	// References
	WorldService* mWorldService = nullptr;
	SAGE::CameraService* mCameraService = nullptr;
	PlayerUIService* mPlayerUIService = nullptr;

	// FOV Zoom
	const float mFov = 60.0f;
	const float mZoomFov = 20.0f;
	bool mIsZoomed = false;
	bool mIsZooming = false;
	void ChangeFOV();

	// Zoom
	SAGE::Coroutine::Enumerator ChangeCameraFOV(const float from, const float to);

	// Mining
	SAGE::Coroutine::Enumerator MineBlock(const SAGE::Math::Vector3 blockPos);
	SAGE::Coroutine::Enumerator MineInstantBlock();
	bool mIsMining = false;
	bool mMinedBlock = false;
	bool mMineInstantBlock = true;
	SAGE::Math::Vector3 mBlockHoverPos = -SAGE::Math::Vector3::One;
	SAGE::Math::Vector3 mClosestPoint;
	SAGE::Math::Vector3 mClosestNormal;
	SAGE::Graphics::TextureId mDestroyStagesTextureIDs[10];

	bool InstantBreakCheck(SAGE::Math::Vector3 closestPoint, SAGE::Math::Vector3 closestNormal);

	SAGE::Coroutine::Enumerator ScreenShake(const float duration, const float magnitude);
};