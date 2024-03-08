#include "PlayerControllerComponent.h"
#include "WorldService.h"
#include "PlayerUIService.h"

#include "MobService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Input;
using namespace SAGE::Coroutine;

MEMORY_POOL_DEFINE(PlayerControllerComponent, 900);

void PlayerControllerComponent::Initialize()
{
	auto& world = GetOwner().GetWorld();
	mWorldService = world.GetService<WorldService>();
	mCameraService = world.GetService<CameraService>();
	mPlayerUIService = world.GetService<PlayerUIService>();

	InitializeSoundIds();

	// Tell the playerUi who the player is
	mPlayerUIService->playerInventory = mInventory;
	mPlayerUIService->SetPlayerHandle(GetOwner().GetHandle());

	// tell the mobservice who the player is
	GetOwner().GetWorld().GetService<MobService>()->RegisterPlayer(GetOwner().GetHandle());

	for (int i = 0; i < 10; ++i)
	{
		mDestroyStagesTextureIDs[i] = TextureManager::Get()->LoadTexture("../Sprites/SageCraft/DestroyStages/destroy_stage_" + std::to_string(i) + ".png");
	}

	mMiningCube.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mMiningCube.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mMiningCube.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mMiningCube.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	mMiningCube.material.power = 10.0f;
	mMiningCube.diffuseMapId = mDestroyStagesTextureIDs[0];
	mMiningCube.meshBuffer.Initialize(MeshBuilder::CreateCube());
	mMiningCube.transform.scale = Vector3(1.005f, 1.005f, 1.005f);
}

void PlayerControllerComponent::Terminate()
{
	mPlayerRB = nullptr;
	mMiningCube.Terminate();
	mPlayerUIService = nullptr;
	mCameraService = nullptr;
	mWorldService = nullptr;
}

void PlayerControllerComponent::Update(float deltaTime)
{
	if (mMinedBlock)
	{
		mMinedBlock = false;
		PlayerLeftClick(mClosestPoint, mClosestNormal);
	}

	MovePlayer(deltaTime);
	BlockHover(mCameraService->GetCamera());
	ChangeFOV();

	auto inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::P)) {
		TakeDamage(1);
	}

	if (mSurvivalMode)
	{
		PassiveHeal(deltaTime);
		PassiveHunger(deltaTime);

		// Eat Food
		if (inputSystem->IsMousePressed(MouseButton::RBUTTON))
		{
			const ItemTypes itemInHand = mInventory[mPlayerUIService->GetIconSelected()].itemType;
			if (IsFood(itemInHand))
			{
				EatFood(itemInHand);
			}
		}
	}

	if (inputSystem->IsMousePressed(MouseButton::LBUTTON)) {
		auto& world = GetOwner().GetWorld();
		const auto dynamicsWorld = world.GetService<BPhysicsService>()->GetDynamicsWorld();

		// Define the start and end points of the ray
		btVector3 rayStart = ConvertToBtvector3(mCameraService->GetCamera().GetPosition());
		btVector3 rayEnd = rayStart + ConvertToBtvector3(mCameraService->GetCamera().GetDirection() * 3.0f);

		// Create a btCollisionWorld::ClosestRayResultCallback object to hold the results of the raycast
		btCollisionWorld::ClosestRayResultCallback rayCallback(rayStart, rayEnd);

		// Perform the raycast by calling the rayTest method on the collision world
		dynamicsWorld->rayTest(rayStart, rayEnd, rayCallback);

		// Check if the ray intersected with any objects
		if (rayCallback.hasHit())
		{
			if (rayCallback.m_collisionObject->isActive())
			{
				const ItemTypes itemInHand = mInventory[mPlayerUIService->GetIconSelected()].itemType;
				const int damage = DamagePoints(itemInHand);
				world.GetService<MobService>()->AttackMob(rayCallback.m_collisionObject->getUserIndex(), damage, ConvertToVector3(rayEnd - rayStart));
			}
		}
	}

	//const auto key = mWorldService->KeyGenerator(mWorldService->mNewWorldCoords);
	//auto pos = GetOwner().GetComponent<TransformComponent>()->position;
	//int x = pos.x;
	//int y = pos.y;
	//int z = pos.z;
	//const BlockInfo blockInfo = mWorldService->mChunkData.at(key).blocksData[x][y][z];
	//
	//if (blockInfo.blockTypes == BlockTypes::Water && blockInfo.padding < 7)
	//{
	//	Vector3 dir;
	//	switch (blockInfo.compass)
	//	{
	//	case DirectionVector::North:
	//		dir = Vector3(0.0f, 0.0f, 1.0f);
	//		break;
	//	case DirectionVector::East:
	//		dir = Vector3(1.0f, 0.0f, 0.0f);
	//		break;
	//	case DirectionVector::South:
	//		dir = Vector3(0.0f, 0.0f, -1.0f);
	//		break;
	//	case DirectionVector::West:
	//		dir = Vector3(-1.0f, 0.0f, 0.0f);
	//		break;
	//	}
	//	mPlayerRB->applyCentralImpulse(ConvertToBtvector3(dir));
	//}
}

void PlayerControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Player Controller Component##PlayerControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::InputFloat("Walk Speed##PlayerControllerComponent", &mWalkSpeed, 0.01f, 0.05f);
		ImGui::InputFloat("Sprint Speed##PlayerControllerComponent", &mSprintSpeed, 0.01f, 0.05f);
		ImGui::InputFloat("Max Speed##PlayerControllerComponent", &mMaxSpeed, 0.01f, 0.05f);
		ImGui::InputFloat("Air Speed##PlayerControllerComponent", &mAirSpeed, 0.01f, 0.05f);
		ImGui::InputFloat("Jump Speed##PlayerControllerComponent", &mJumpSpeed, 0.01f, 0.05f);
		ImGui::InputFloat("gravity##PlayerControllerComponent", &mGravity, 0.01f, 0.05f);
		if (ImGui::Checkbox("Enable Survival Mode", &mSurvivalMode))
		{
			mPlayerRB->setGravity(mSurvivalMode ? btVector3(0.0f, mGravity, 0.0f) : btVector3(0.0f, 0.0f, 0.0f));
		}
	}
}

void PlayerControllerComponent::OnEnable()
{
	mPlayerRB = GetOwner().GetComponent<RigidBodyComponent>()->GetRigidBody();
	mPlayerRB->setActivationState(DISABLE_DEACTIVATION);
	mPlayerRB->setGravity(mSurvivalMode ? btVector3(0.0f, mGravity, 0.0f) : btVector3(0.0f, 0.0f, 0.0f));
	//playerRBC->AddCollisionMask(CollisionGroups::Group1);

	auto tc = GetOwner().GetComponent<TransformComponent>();
	tc->SetPosition(tc->position + Vector3::YAxis);
}

void PlayerControllerComponent::TakeDamage(int dmg)
{
	if (dmg <= 0)
	{
		return;
	}

	mHealth -= dmg;
	mHealth = Clamp(mHealth, 0, 20);

	mPlayerUIService->UpdatePlayerHp(static_cast<float>(mHealth) * 0.05f);
	CoroutineSystem::Get()->StartCoroutine(ScreenShake(0.05f, 0.075f));

	if (mHealth <= 0)
	{
		Respawn();
	}
}

void PlayerControllerComponent::GainHealth(int hp)
{
	mHealth += hp;
	mHealth = Clamp(mHealth, 0, 20);

	mPlayerUIService->UpdatePlayerHp(static_cast<float>(mHealth) * 0.05f);
}

void PlayerControllerComponent::Respawn()
{
	mHealth = mMaxHealth;
	mHunger = mMaxHunger;
	mPlayerUIService->UpdatePlayerHp(1.0f);
	mPlayerUIService->UpdatePlayerHunger(1.0f);

	const std::string chunkKey = mWorldService->KeyGenerator(Vector2Int(0, 0));
	const auto& chunk = mWorldService->mChunkData.at(chunkKey);
	float maxYSpawn = 32.0f;
	for (int i = 0; i < 31; ++i)
	{
		if (chunk.blocksData[8][i][8].blockTypes == BlockTypes::Air 
			&& chunk.blocksData[8][i + 1][8].blockTypes == BlockTypes::Air)
		{
			maxYSpawn = static_cast<float>(i) + 1.5f;
			break;
		}
	}

	GetOwner().GetComponent<TransformComponent>()->SetPosition(Vector3(0.5f, maxYSpawn, 0.5f));
}

void PlayerControllerComponent::PassiveHeal(float deltaTime)
{
	mHealthTickTimer += deltaTime;
	while (mHealthTickTimer > mHealthTickStep)
	{
		mHealthTickTimer -= mHealthTickStep;
		if (mHunger >= mHungerThreshold && mHealth < mMaxHealth)
		{
			GainHealth(1);
		}
	}
}

void PlayerControllerComponent::PassiveHunger(float deltaTime)
{
	mHungerTickTimer += deltaTime;
	while (mHungerTickTimer > mHungerTickStep)
	{
		mHungerTickTimer -= mHungerTickStep;
		mHunger = Clamp(--mHunger, 0, mMaxHunger);

		mPlayerUIService->UpdatePlayerHunger(static_cast<float>(mHunger) * 0.05f);
		if (mHunger <= 0)
		{
			TakeDamage(1);
		}
	}
}

void PlayerControllerComponent::EatFood(ItemTypes foodItem)
{
	if (mHunger >= mMaxHunger)
	{
		return;
	}

	// Consume the food
	if (mSurvivalMode)
	{
		if (--mInventory[mPlayerUIService->GetIconSelected()].amount <= 0)
		{
			mInventory[mPlayerUIService->GetIconSelected()].EmptySlot();
		}
	}

	// Increase hunger points
	mHunger += FoodPoints(foodItem);
	mHunger = Clamp(mHunger, 0, mMaxHunger);
	mPlayerUIService->UpdatePlayerHunger(static_cast<float>(mHunger) * 0.05f);
}

void PlayerControllerComponent::MovePlayer(float deltaTime)
{
	auto& cam = GetOwner().GetWorld().GetService<CameraService>()->GetCamera();
	cam.SetPosition(GetOwner().GetComponent<TransformComponent>()->position + Vector3(0.0f, 1.75f, 0.0f));

	Vector3 forward = cam.GetDirection();
	const Vector3 right = Cross(Vector3::YAxis, forward);

	if (!mPlayerUIService->IsUIShowing())
	{
		mSurvivalMode ? SurvivalMovePlayer(deltaTime, forward, right) : CreativeMovePlayer(deltaTime, forward, right);
	}
}

void PlayerControllerComponent::SurvivalMovePlayer(float deltaTime, Vector3& forward, const Vector3& right)
{
	auto inputSystem = InputSystem::Get();
	forward.y = 0.0f;
	mLinearVelocity = SAGE::Math::Vector3::Zero; // Reset Velocity
	const bool isOnGround = GroundCheck();

	const bool isSprinting = inputSystem->IsKeyDown(KeyCode::LSHIFT);
	const float moveSpeed = isSprinting ? mSprintSpeed : mWalkSpeed;

	if (!mIsZooming && !mIsZoomed)
	{
		mCameraService->GetCamera().SetFov((isSprinting ? 59.0f : mFov) * Constants::DegToRad);
	}

	bool isMoving = false;

	// Forward movement
	if (inputSystem->IsKeyDown(KeyCode::W))
	{
		isMoving = true;
		mLinearVelocity += forward * (deltaTime * (isOnGround ? moveSpeed : mAirSpeed));
	}

	if (inputSystem->IsKeyDown(KeyCode::S))
	{
		isMoving = true;
		mLinearVelocity += forward * (-deltaTime * (isOnGround ? moveSpeed : mAirSpeed));
	}

	// Sideways movement
	if (inputSystem->IsKeyDown(KeyCode::A))
	{
		isMoving = true;
		mLinearVelocity += right * (-deltaTime * (isOnGround ? moveSpeed : mAirSpeed));
	}

	if (inputSystem->IsKeyDown(KeyCode::D))
	{
		isMoving = true;
		mLinearVelocity += right * (deltaTime * (isOnGround ? moveSpeed : mAirSpeed));
	}

	// Jump
	if (isOnGround && inputSystem->IsKeyPressed(KeyCode::SPACE))
	{
		isMoving = true;
		mLinearVelocity.y += mJumpSpeed;
	}

	// Get the current velocity of the rigid body
	btVector3 velocity = mPlayerRB->getLinearVelocity();

	if ((velocity.x() <= -mMaxSpeed && mLinearVelocity.x < 0.0f) || (velocity.x() >= mMaxSpeed && mLinearVelocity.x > 0.0f))
	{
		mLinearVelocity.x = 0.0f;
	}

	if ((velocity.z() <= -mMaxSpeed && mLinearVelocity.z < 0.0f) || (velocity.z() >= mMaxSpeed && mLinearVelocity.z > 0.0f))
	{
		mLinearVelocity.z = 0.0f;
	}

	mPlayerRB->applyCentralImpulse(ConvertToBtvector3(mLinearVelocity));

	if (!isMoving && isOnGround)
	{
		DeceleratePlayer();
	}
}

void PlayerControllerComponent::CreativeMovePlayer(float deltaTime, Vector3& forward, const Vector3& right)
{
	auto inputSystem = InputSystem::Get();
	mLinearVelocity = SAGE::Math::Vector3::Zero; // Reset Velocity

	const bool isOnGround = GroundCheck();
	const bool isSprinting = inputSystem->IsKeyDown(KeyCode::LSHIFT);
	const float moveSpeed = isSprinting ? mSprintSpeed : mWalkSpeed;

	if (!mIsZooming && !mIsZoomed)
	{
		mCameraService->GetCamera().SetFov((isSprinting ? 59.0f : mFov) * Constants::DegToRad);
	}

	bool isMoving = false;

	// Forward movement
	if (inputSystem->IsKeyDown(KeyCode::W))
	{
		isMoving = true;
		mLinearVelocity += forward * (deltaTime * (isOnGround ? moveSpeed : mAirSpeed));
	}

	if (inputSystem->IsKeyDown(KeyCode::S))
	{
		isMoving = true;
		mLinearVelocity += forward * (-deltaTime * (isOnGround ? moveSpeed : mAirSpeed));
	}

	// Sideways movement
	if (inputSystem->IsKeyDown(KeyCode::A))
	{
		isMoving = true;
		mLinearVelocity += right * (-deltaTime * (isOnGround ? moveSpeed : mAirSpeed));
	}

	if (inputSystem->IsKeyDown(KeyCode::D))
	{
		isMoving = true;
		mLinearVelocity += right * (deltaTime * (isOnGround ? moveSpeed : mAirSpeed));
	}

	// Jump
	if (inputSystem->IsKeyDown(KeyCode::SPACE))
	{
		isMoving = true;
		mLinearVelocity.y += mJumpSpeed * 5.0f * deltaTime;
	}

	// Get the current velocity of the rigid body
	btVector3 velocity = mPlayerRB->getLinearVelocity();

	mPlayerRB->setLinearVelocity(ConvertToBtvector3(mLinearVelocity * 20.0f));

	if (!isMoving)
	{
		DeceleratePlayer();
	}
}

bool PlayerControllerComponent::GroundCheck()
{
	const auto dynamicsWorld = GetOwner().GetWorld().GetService<BPhysicsService>()->GetDynamicsWorld();
	const float extend = GetOwner().GetComponent<ColliderComponent>()->GetExtend().x;

	for (int z = -1; z <= 1; ++z)
	{
		for (int x = -1; x <= 1; ++x)
		{
			// Define the start and end points of the ray
			btVector3 rayStart = mPlayerRB->getWorldTransform().getOrigin() + btVector3(static_cast<btScalar>(x * extend), static_cast<btScalar>(0.0f), static_cast<btScalar>(z * extend));
			btVector3 rayEnd = rayStart - btVector3(static_cast<btScalar>(0.0f), static_cast<btScalar>(1.25f), static_cast<btScalar>(0.0f)); // 1.25 = rayLength is the length of the ray

			// Create a btCollisionWorld::ClosestRayResultCallback object to hold the results of the raycast
			btCollisionWorld::ClosestRayResultCallback rayCallback(rayStart, rayEnd);

			// Perform the raycast by calling the rayTest method on the collision world
			dynamicsWorld->rayTest(rayStart, rayEnd, rayCallback);

			// Check if the ray intersected with any objects
			if (rayCallback.hasHit())
			{
				if (rayCallback.m_collisionObject->isStaticObject())
				{
					UpdateFalling(false);
					return true;
				}
			}
		}
	}

	UpdateFalling(true);
	return false;
}

void PlayerControllerComponent::UpdateFalling(bool isFalling)
{
	if (mIsFalling == true && isFalling == false) // Landed
	{
		float fallAmount = Max(0.0f, mStartFallYValue - mCameraService->GetCamera().GetPosition().y - 3.0f);
		if (fallAmount >= 1.0f)
		{
			TakeDamage(static_cast<int>(fallAmount + 0.5f));
		}
	}
	else if (mIsFalling == false && isFalling == true) // Started falling
	{
		mStartFallYValue = mCameraService->GetCamera().GetPosition().y;
	}

	mIsFalling = isFalling;
}

void PlayerControllerComponent::DeceleratePlayer()
{
	// Get the current velocity of the rigid body
	btVector3 velocity = mPlayerRB->getLinearVelocity();

	// Calculate the braking force required to stop the rigid body within a specified time
	btScalar mass = mPlayerRB->getMass();
	btScalar brakingMultiplyer = 10.0f;
	btVector3 brakingForce = -velocity * mass * brakingMultiplyer;

	// Apply the braking force to the rigid body
	mPlayerRB->applyCentralForce(brakingForce * btVector3(1.0f, 0.0f, 1.0f));
}

void PlayerControllerComponent::BlockHover(const Camera& camera)
{
	auto inputSystem = InputSystem::Get();
	Ray ray = { camera.GetPosition(), (camera.GetDirection()) };

	std::vector<Vector3> points;
	std::vector<Vector3> normals;
	std::vector<Vector3> aabbsPos;

	for (auto& aabb : mWorldService->mAABBs)
	{
		//Block Breaking Distance
		if (Distance(camera.GetPosition(), aabb.center) > 8.5f) { continue; }

		Vector3 point;
		Vector3 normal;
		OBB obb = { aabb.center, aabb.extend, Quaternion::Identity };
		if (Intersect(ray, obb, point, normal))
		{
			points.push_back(point);
			normals.push_back(normal);
			aabbsPos.push_back(aabb.center);
		}
	}

	if (points.empty()) {
		return;
	}

	Vector3 closestPoint = points[0];
	Vector3 closestNormal = normals[0];
	Vector3 closestAABBPos = aabbsPos[0];

	for (int i = 1; i < points.size(); ++i)
	{
		if (Distance(camera.GetPosition(), points[i]) < Distance(camera.GetPosition(), closestPoint))
		{
			closestPoint = points[i];
			closestNormal = normals[i];
			closestAABBPos = aabbsPos[i];
		}
	}

	mBlockHoverPos = closestAABBPos;
	SimpleDraw::AddAABB(closestAABBPos, { 0.505f, 0.505f , 0.505f }, Colors::White);

	if (mPlayerUIService->IsUIShowing())
	{
		return;
	}

	if (inputSystem->IsMouseDown(MouseButton::LBUTTON)) {
		if (mSurvivalMode)
		{
			if (mIsMining)
			{
				return;
			}

			if (InstantBreakCheck(closestPoint, closestNormal))
			{
				mIsMining = true;
				mClosestPoint = closestPoint;
				mClosestNormal = closestNormal;
				CoroutineSystem::Get()->StartCoroutine(MineInstantBlock());
				return;
			}

			mIsMining = true;
			mClosestPoint = closestPoint;
			mClosestNormal = closestNormal;
			CoroutineSystem::Get()->StartCoroutine(MineBlock(mBlockHoverPos));
		}
		else
		{
			if (inputSystem->IsMousePressed(MouseButton::LBUTTON) || inputSystem->IsKeyDown(KeyCode::LSHIFT))
			{
				PlayerLeftClick(closestPoint, closestNormal);
			}
		}
	}
	else if (inputSystem->IsMousePressed(MouseButton::MBUTTON)) {
		PlayerMiddleClick(closestPoint, closestNormal);
	}
	else if (inputSystem->IsMousePressed(MouseButton::RBUTTON)) {
		PlayerRightClick(closestPoint, closestNormal);
	}
}

void PlayerControllerComponent::PlayerLeftClick(Vector3 closestPoint, Vector3 closestNormal)
{
	const Vector2Int closestPointCoords = GetClosestPointCoords(closestPoint, closestNormal);

	const std::string key = mWorldService->KeyGenerator(closestPointCoords);
	if (mWorldService->mChunkData.find(key) != mWorldService->mChunkData.end()) {
		auto block = mWorldService->mChunkData.at(key).GetBlock(closestPointCoords, closestPoint);
		if (block.y == 0) { return; }

		const BlockTypes blockType = mWorldService->mChunkData.at(key).blocksData[block.x][block.y][block.z].blockTypes;
		// Add To Inventory
		if (mSurvivalMode)
		{
			AddItemToInventory(BlockTypeToItemType(blockType));
		}

		// Update Chunks
		if (mWorldService->mChunkData.at(key).SetBlock(block, BlockInfo(BlockTypes::Air)))
		{
			mWorldService->UpdateChunk(closestPointCoords); // Update chunk

			// Update surrounding chunks if block removed was on the edge of a chunk
			if (block.x == 0) {
				mWorldService->UpdateChunk({ closestPointCoords.x - 1, closestPointCoords.y });
			}
			else if (block.x == 15) {
				mWorldService->UpdateChunk({ closestPointCoords.x + 1, closestPointCoords.y });
			}

			if (block.z == 0) {
				mWorldService->UpdateChunk({ closestPointCoords.x, closestPointCoords.y - 1 });
			}
			else if (block.z == 15) {
				mWorldService->UpdateChunk({ closestPointCoords.x, closestPointCoords.y + 1 });
			}

			const auto& playerParticleComponent = GetOwner().GetComponent<ParticleComponent>();
			const Color particleColor = GetBlockColor(blockType);
			for (int i = 0; i < 10; ++i)
			{
				ParticleData particle;
				particle.SetPosition(closestPoint);
				particle.SetVelocity(Random::OnUnitSphere() * 0.02f);
				particle.lifeLength = 0.5f;
				particle.gravityEffect = 0.3f;

				particle.currentColor = particleColor;

				particle.scaleOverTime = true;
				particle.startScale = 0.3f;
				particle.endScale = 0.0f;
				particle.currentScale = particle.startScale;

				particle.rotateOverTime = true;
				particle.endRotation = Random::UniformFloat(-180.0f, 180.0f);

				playerParticleComponent->AddParticle(particle);
			}

			PlayBlockBreakingSound(blockType);
			mWorldService->UpdateAABBS();
		}
	}
}

void PlayerControllerComponent::PlayerMiddleClick(Vector3 closestPoint, Vector3 closestNormal)
{
	const Vector2Int closestPointCoords = GetClosestPointCoords(closestPoint, closestNormal);

	const std::string key = mWorldService->KeyGenerator(closestPointCoords);
	if (mWorldService->mChunkData.find(key) != mWorldService->mChunkData.end()) {
		auto block = mWorldService->mChunkData.at(key).GetBlock(closestPointCoords, closestPoint);

		BlockTypes blockType = mWorldService->mChunkData.at(key).blocksData[block.x][block.y][block.z].blockTypes;

		if (mSurvivalMode) // If item exist in hotbar move hotbar to it
		{
			for (int i = 0; i < 9; ++i)
			{
				if (mInventory[i].itemType == BlockTypeToItemType(blockType))
				{
					mPlayerUIService->UpdateIconLocation(i);
					return;
				}
			}
		}
		else // Turn slot into that block
		{
			mInventory[mPlayerUIService->GetIconSelected()].itemType = BlockTypeToItemType(blockType);
			mInventory[mPlayerUIService->GetIconSelected()].amount = 64;
		}
	}
}

void PlayerControllerComponent::PlayerRightClick(Vector3 closestPoint, SAGE::Math::Vector3 closestNormal)
{
	// Interacting with a block
	if (InteractWithBlock(closestPoint, closestNormal))
	{
		return;
	}

	// Cant place a block you dont have
	const ItemTypes itemInHand = mInventory[mPlayerUIService->GetIconSelected()].itemType;
	if (itemInHand == ItemTypes::None)
	{
		return;
	}

	if (IsHoe(itemInHand))
	{
		Hoeing(closestPoint, closestNormal);
		return;
	}

	Vector2Int closestPointCoords = GetClosestPointCoords(closestPoint, closestNormal, false);

	if (closestPointCoords == Vector2Int(-1, -1))
	{
		return;
	}

	const std::string key = mWorldService->KeyGenerator(closestPointCoords);
	if (mWorldService->mChunkData.find(key) != mWorldService->mChunkData.end()) {
		auto& chunkData = mWorldService->mChunkData.at(key);
		Vector3Int blockCoords = chunkData.GetBlock(closestPointCoords, closestPoint);

		if (chunkData.blocksData[blockCoords.x][blockCoords.y][blockCoords.z].blockTypes != BlockTypes::Air)
		{
			return;
		}

		const BlockTypes itemToPlaceType = ItemTypeToBlockType(itemInHand);

		if (itemToPlaceType == BlockTypes::Air)
		{
			return;
		}

		bool isFloored = closestPoint.y < mBlockHoverPos.y;
		if (closestNormal == Vector3::YAxis) {
			isFloored = true;
		}
		else if (closestNormal == -Vector3::YAxis) {
			isFloored = false;
		}

		if (chunkData.SetBlock(blockCoords, BlockInfo(itemToPlaceType, isFloored, itemToPlaceType == BlockTypes::Water ? static_cast<int8_t>(7) : static_cast<int8_t>(0)))) /// TODO: Get a padding amount specified per item
		{
			if (mSurvivalMode)
			{
				if (--mInventory[mPlayerUIService->GetIconSelected()].amount <= 0)
				{
					mInventory[mPlayerUIService->GetIconSelected()].EmptySlot();
				}
			}

			mWorldService->UpdateChunk(closestPointCoords);

			// Update surrounding chunks if block removed was on the edge of a chunk
			if (blockCoords.x == 0) {
				mWorldService->UpdateChunk({ closestPointCoords.x - 1, closestPointCoords.y });
			}
			else if (blockCoords.x == 15) {
				mWorldService->UpdateChunk({ closestPointCoords.x + 1, closestPointCoords.y });
			}

			if (blockCoords.z == 0) {
				mWorldService->UpdateChunk({ closestPointCoords.x, closestPointCoords.y - 1 });
			}
			else if (blockCoords.z == 15) {
				mWorldService->UpdateChunk({ closestPointCoords.x, closestPointCoords.y + 1 });
			}

			PlayBlockPlacingSound(itemToPlaceType);
			mWorldService->UpdateAABBS();
		}
	}
}

void PlayerControllerComponent::Hoeing(Vector3 closestPoint, SAGE::Math::Vector3 closestNormal)
{
	const Vector2Int closestPointCoords = GetClosestPointCoords(closestPoint, closestNormal);

	const std::string key = mWorldService->KeyGenerator(closestPointCoords);
	if (mWorldService->mChunkData.find(key) != mWorldService->mChunkData.end()) {
		auto block = mWorldService->mChunkData.at(key).GetBlock(closestPointCoords, closestPoint);
		if (block.y == 0) { return; }

		const BlockTypes blockType = mWorldService->mChunkData.at(key).blocksData[block.x][block.y][block.z].blockTypes;

		// Cant Hoe 
		if (blockType != BlockTypes::GrassBlock && blockType != BlockTypes::SnowGrassBlock)
		{
			return;
		}

		// Update Chunks
		if (mWorldService->mChunkData.at(key).SetBlock(block, BlockInfo(BlockTypes::Farmland)))
		{
			mWorldService->UpdateChunk(closestPointCoords); // Update chunk

			// Update surrounding chunks if block removed was on the edge of a chunk
			if (block.x == 0) {
				mWorldService->UpdateChunk({ closestPointCoords.x - 1, closestPointCoords.y });
			}
			else if (block.x == 15) {
				mWorldService->UpdateChunk({ closestPointCoords.x + 1, closestPointCoords.y });
			}

			if (block.z == 0) {
				mWorldService->UpdateChunk({ closestPointCoords.x, closestPointCoords.y - 1 });
			}
			else if (block.z == 15) {
				mWorldService->UpdateChunk({ closestPointCoords.x, closestPointCoords.y + 1 });
			}

			PlayBlockPlacingSound(BlockTypes::Farmland);
			mWorldService->UpdateAABBS();
		}
	}

}

bool PlayerControllerComponent::InteractWithBlock(Vector3 closestPoint, SAGE::Math::Vector3 closestNormal)
{
	if (!InputSystem::Get()->IsKeyDown(KeyCode::LSHIFT))
	{
		Vector2Int closestPointCoords = GetClosestPointCoords(closestPoint, closestNormal, true);
		const std::string key = mWorldService->KeyGenerator(closestPointCoords);

		if (mWorldService->mChunkData.find(key) != mWorldService->mChunkData.end())
		{
			const auto block = mWorldService->mChunkData.at(key).GetBlock(closestPointCoords, closestPoint);
			//const BlockTypes blockType = mWorldService->mChunkData.at(key).blocksData[block.x][block.y][block.z];
			const BlockTypes blockType = mWorldService->mChunkData.at(key).blocksData[block.x][block.y][block.z].blockTypes;

			switch (blockType)
			{
			case BlockTypes::CraftingTable:
				mPlayerUIService->SwitchUIScreen(UIScreens::CraftingTable);
				return true;
			case BlockTypes::Furnace:
				mPlayerUIService->SwitchUIScreen(UIScreens::Furnace);
				return true;
			}
		}
	}
	return false;
}

Vector2Int PlayerControllerComponent::GetClosestPointCoords(Vector3& closestPoint, Vector3 closestNormal, bool isLeftClick)
{
	if (closestNormal == Vector3::XAxis) {
		closestPoint.x += isLeftClick ? -0.5f : 0.5f;
	}
	else if (closestNormal == -Vector3::XAxis) {
		closestPoint.x += isLeftClick ? 0.5f : -0.5f;
	}
	else if (closestNormal == Vector3::YAxis) {
		closestPoint.y += isLeftClick ? -0.5f : 0.5f;
		if (!isLeftClick && closestPoint.y > 32.0f) { return Vector2Int(-1, -1); }
	}
	else if (closestNormal == -Vector3::YAxis) {
		closestPoint.y += isLeftClick ? 0.5f : -0.5f;
		if (!isLeftClick && closestPoint.y < 0.0f) { return Vector2Int(-1, -1); }
	}
	else if (closestNormal == Vector3::ZAxis) {
		closestPoint.z += isLeftClick ? -0.5f : 0.5f;
	}
	else if (closestNormal == -Vector3::ZAxis) {
		closestPoint.z += isLeftClick ? 0.5f : -0.5f;
	}

	return Vector2Int((static_cast<int>(closestPoint.x) + (closestPoint.x > 0.0f ? 8 : -8)) / 16, (static_cast<int>(closestPoint.z) + (closestPoint.z > 0.0f ? 8 : -8)) / 16);
}

void PlayerControllerComponent::AddItemToInventory(ItemTypes itemType)
{
	// Existing slot with free space
	for (int i = 0; i < 36; ++i)
	{
		if (mInventory[i].itemType == itemType && mInventory[i].amount < 64)
		{
			++mInventory[i].amount;
			return;
		}
	}

	// Free slot
	for (int i = 0; i < 36; ++i)
	{
		if (mInventory[i].itemType == ItemTypes::None)
		{
			mInventory[i].itemType = itemType;
			++mInventory[i].amount;
			return;
		}
	}
}

ItemTypes PlayerControllerComponent::BlockTypeToItemType(const BlockTypes blockType)
{
	switch (blockType)
	{
	case BlockTypes::Air: return ItemTypes::None;
	case BlockTypes::BedRock: return ItemTypes::BedRock;
	case BlockTypes::GrassBlock: return ItemTypes::GrassBlock;
	case BlockTypes::Dirt: return ItemTypes::Dirt;
	case BlockTypes::CobbleStone: return ItemTypes::CobbleStone;
	case BlockTypes::Stone: return ItemTypes::Stone;
	case BlockTypes::OakLeaves: return ItemTypes::OakLeaves;
	case BlockTypes::OakLog: return ItemTypes::OakLog;
	case BlockTypes::OakPlank: return ItemTypes::OakPlank;
	case BlockTypes::Glass: return ItemTypes::Glass;
	case BlockTypes::Sand: return ItemTypes::Sand;
	case BlockTypes::Gravel: return ItemTypes::Gravel;
	case BlockTypes::CoalOre: return ItemTypes::CoalOre;
	case BlockTypes::IronOre: return ItemTypes::IronOre;
	case BlockTypes::GoldOre: return ItemTypes::GoldOre;
	case BlockTypes::DiamondOre: return ItemTypes::DiamondOre;
	case BlockTypes::RedStoneOre: return ItemTypes::RedStoneOre;
	case BlockTypes::LapisOre: return ItemTypes::LapisOre;
	case BlockTypes::EmeraldOre: return ItemTypes::EmeraldOre;
	case BlockTypes::SnowGrassBlock: return ItemTypes::SnowGrassBlock;
	case BlockTypes::Snow: return ItemTypes::Snow;
	case BlockTypes::Cactus: return ItemTypes::Cactus;
	case BlockTypes::CraftingTable: return ItemTypes::CraftingTable;
	case BlockTypes::Furnace: return ItemTypes::Furnace;
	case BlockTypes::OakSlab: return ItemTypes::OakSlab;
	case BlockTypes::CobbleStoneSlab: return ItemTypes::CobbleStoneSlab;
	case BlockTypes::StoneSlab: return ItemTypes::StoneSlab;
	case BlockTypes::Grass: return ItemTypes::Grass;
	case BlockTypes::Poppy: return ItemTypes::Poppy;
	case BlockTypes::Dandelion: return ItemTypes::Dandelion;
	case BlockTypes::SugarCane: return ItemTypes::SugarCane;
	case BlockTypes::Lava: return ItemTypes::Lava;
	case BlockTypes::Water: return ItemTypes::Water;
	case BlockTypes::Companion: return ItemTypes::Companion;
	case BlockTypes::Farmland: return ItemTypes::Dirt;
	case BlockTypes::Wheat: return ItemTypes::Wheat;
	default:
		return ItemTypes::None;
	}
}

BlockTypes PlayerControllerComponent::ItemTypeToBlockType(const ItemTypes itemType)
{
	switch (itemType)
	{
	case ItemTypes::BedRock: return BlockTypes::BedRock;
	case ItemTypes::GrassBlock: return BlockTypes::GrassBlock;
	case ItemTypes::Dirt: return BlockTypes::Dirt;
	case ItemTypes::CobbleStone: return BlockTypes::CobbleStone;
	case ItemTypes::Stone: return BlockTypes::Stone;
	case ItemTypes::OakLeaves: return BlockTypes::OakLeaves;
	case ItemTypes::OakLog: return BlockTypes::OakLog;
	case ItemTypes::OakPlank: return BlockTypes::OakPlank;
	case ItemTypes::Glass: return BlockTypes::Glass;
	case ItemTypes::Sand: return BlockTypes::Sand;
	case ItemTypes::Gravel: return BlockTypes::Gravel;
	case ItemTypes::CoalOre: return BlockTypes::CoalOre;
	case ItemTypes::IronOre: return BlockTypes::IronOre;
	case ItemTypes::GoldOre: return BlockTypes::GoldOre;
	case ItemTypes::DiamondOre: return BlockTypes::DiamondOre;
	case ItemTypes::RedStoneOre: return BlockTypes::RedStoneOre;
	case ItemTypes::LapisOre: return BlockTypes::LapisOre;
	case ItemTypes::EmeraldOre: return BlockTypes::EmeraldOre;
	case ItemTypes::SnowGrassBlock: return BlockTypes::SnowGrassBlock;
	case ItemTypes::Snow: return BlockTypes::Snow;
	case ItemTypes::Cactus: return BlockTypes::Cactus;
	case ItemTypes::CraftingTable: return BlockTypes::CraftingTable;
	case ItemTypes::Furnace: return BlockTypes::Furnace;
	case ItemTypes::OakSlab: return BlockTypes::OakSlab;
	case ItemTypes::CobbleStoneSlab: return BlockTypes::CobbleStoneSlab;
	case ItemTypes::StoneSlab: return BlockTypes::StoneSlab;
	case ItemTypes::Grass: return BlockTypes::Grass;
	case ItemTypes::Poppy: return BlockTypes::Poppy;
	case ItemTypes::Dandelion: return BlockTypes::Dandelion;
	case ItemTypes::SugarCane: return BlockTypes::SugarCane;
	case ItemTypes::Lava: return BlockTypes::Lava;
	case ItemTypes::Water: return BlockTypes::Water;
	case ItemTypes::Companion: return BlockTypes::Companion;
	case ItemTypes::Wheat: return BlockTypes::Wheat;
	default:
		return BlockTypes::Air;
	}
}

void PlayerControllerComponent::PlayBlockBreakingSound(const BlockTypes blockType)
{
	SoundId soundFx;

	switch (blockType)
	{
	case BlockTypes::Glass: soundFx = mGlassBreakID; break;
	case BlockTypes::OakLeaves:
	case BlockTypes::Grass:
	case BlockTypes::Poppy:
	case BlockTypes::Dandelion:
	case BlockTypes::SugarCane:
	case BlockTypes::Wheat:
		soundFx = mLeafBreakID; break;
	default:
		soundFx = mCrunchID;
		break;
	}

	SoundEffectManager::Get()->Play(soundFx, false, 1.0f, Random::UniformFloat(-0.1f, 0.15f), Random::UniformFloat(-0.1f, 0.1f));
}

void PlayerControllerComponent::PlayBlockPlacingSound(const BlockTypes blockType)
{
	SoundId soundFx;

	switch (blockType)
	{
	case BlockTypes::OakLeaves:
	case BlockTypes::Grass:
	case BlockTypes::Poppy:
	case BlockTypes::Dandelion:
	case BlockTypes::SugarCane:
	case BlockTypes::Wheat:
		soundFx = mLeafBreakID; break;
	case BlockTypes::Farmland:
		soundFx = mHoeID; break;
	default:
		soundFx = mCrunchID;
		break;
	}

	SoundEffectManager::Get()->Play(soundFx, false, 1.0f, Random::UniformFloat(-0.1f, 0.15f), Random::UniformFloat(-0.1f, 0.1f));
}

void PlayerControllerComponent::InitializeSoundIds()
{
	auto soundEffectManager = SoundEffectManager::Get();
	mCrunchID = soundEffectManager->Load("Crunch.wav");
	mGlassBreakID = soundEffectManager->Load("glass_breaking.wav");
	mLeafBreakID = soundEffectManager->Load("LeafCrunch.wav");
	mHoeID = soundEffectManager->Load("hoe.wav");
}

Color PlayerControllerComponent::GetBlockColor(const BlockTypes blockType)
{
	switch (blockType) // alphabetical
	{
	case BlockTypes::Dirt:
	case BlockTypes::OakLog:
	case BlockTypes::CraftingTable:
		return Colors::SaddleBrown;
	case BlockTypes::GrassBlock:
	case BlockTypes::Grass:
	case BlockTypes::Poppy:
	case BlockTypes::Dandelion:
	case BlockTypes::SugarCane:
	case BlockTypes::OakLeaves:
	case BlockTypes::Cactus:
	case BlockTypes::Wheat:
		return Colors::DarkGreen;
	case BlockTypes::CobbleStone:
	case BlockTypes::Stone:
	case BlockTypes::CoalOre:
	case BlockTypes::IronOre:
	case BlockTypes::GoldOre:
	case BlockTypes::DiamondOre:
	case BlockTypes::RedStoneOre:
	case BlockTypes::LapisOre:
	case BlockTypes::EmeraldOre:
	case BlockTypes::CobbleStoneSlab:
	case BlockTypes::StoneSlab:
	case BlockTypes::Furnace:
		return Colors::Gray;
	case BlockTypes::Water:
		return Colors::LightBlue;
	case BlockTypes::Lava:
		return Colors::Red;
	case BlockTypes::OakPlank:
	case BlockTypes::OakSlab:
	case BlockTypes::Sand:
		return Colors::SandyBrown;
	case BlockTypes::SnowGrassBlock:
	case BlockTypes::Snow:
	case BlockTypes::Glass:
		return Colors::White;
	default:
		return Colors::Pink;
	}
}

void PlayerControllerComponent::ChangeFOV()
{
	if (InputSystem::Get()->IsKeyPressed(KeyCode::LCONTROL))
	{
		if (mIsZooming)
		{
			return;
		}

		mIsZoomed = !mIsZoomed;
		CoroutineSystem::Get()->StartCoroutine(ChangeCameraFOV(mIsZoomed ? mFov : mZoomFov, mIsZoomed ? mZoomFov : mFov));
	}
}

Enumerator PlayerControllerComponent::ChangeCameraFOV(const float from, const float to)
{
	return [=](CoroPush& yield_return)
	{
		mIsZooming = true;
		const auto start = std::chrono::system_clock::now();
		const float totalTime = 0.3f;
		float time = 0.0f;

		while (time < totalTime)
		{
			const auto end = std::chrono::system_clock::now();
			const std::chrono::duration<float> elapsedTime = end - start;
			time = elapsedTime.count();

			const float easeOutSine = sin((time / totalTime * Constants::Pi) * 0.5f);;
			const float fovAmount = Lerp(from, to, easeOutSine);

			mCameraService->GetCamera().SetFov(fovAmount * Constants::DegToRad);
			yield_return(nullptr);
		}
		mCameraService->GetCamera().SetFov(to * Constants::DegToRad);
		mIsZooming = false;
	};
}

Enumerator PlayerControllerComponent::MineBlock(const Vector3 blockPos)
{
	return [=](CoroPush& yield_return)
	{
		mMineInstantBlock = false;
		auto inputSystem = InputSystem::Get();
		const auto start = std::chrono::system_clock::now();
		const float totalTime = 0.75f; // Default time to break a dirt block
		float time = 0.0f;
		mMiningCube.transform.position = blockPos;

		while (time < totalTime && inputSystem->IsMouseDown(MouseButton::LBUTTON))
		{
			// Looked away
			if (mBlockHoverPos.x != blockPos.x || mBlockHoverPos.y != blockPos.y || mBlockHoverPos.z != blockPos.z)
			{
				break;
			}
			const auto end = std::chrono::system_clock::now();
			const std::chrono::duration<float> elapsedTime = end - start;
			time = elapsedTime.count();

			// Block break Progress
			mMiningCube.diffuseMapId = mDestroyStagesTextureIDs[Lerp(0, 9, time / totalTime)];

			yield_return(nullptr);
		}

		// Mined Block
		if (time >= totalTime)
		{
			mMinedBlock = true;
		}

		mIsMining = false;
	};
}

Enumerator PlayerControllerComponent::MineInstantBlock()
{
	return [=](CoroPush& yield_return)
	{
		mMineInstantBlock = true;
		yield_return(new WaitForSeconds(0.1f));
		mMinedBlock = true;
		mIsMining = false;
	};
}

bool PlayerControllerComponent::InstantBreakCheck(Vector3 closestPoint, Vector3 closestNormal)
{
	const Vector2Int closestPointCoords = GetClosestPointCoords(closestPoint, closestNormal);

	const std::string key = mWorldService->KeyGenerator(closestPointCoords);
	if (mWorldService->mChunkData.find(key) != mWorldService->mChunkData.end()) {
		auto block = mWorldService->mChunkData.at(key).GetBlock(closestPointCoords, closestPoint);
		if (block.y == 0) { return false; }

		//const BlockTypes blockType = mWorldService->mChunkData.at(key).blocksData[block.x][block.y][block.z];
		const BlockTypes blockType = mWorldService->mChunkData.at(key).blocksData[block.x][block.y][block.z].blockTypes;

		return IsInstantBreak(blockType);
	}

	return false;
}

Enumerator PlayerControllerComponent::ScreenShake(const float duration, const float magnitude)
{
	return [=](CoroPush& yield_return)
	{
		const auto start = std::chrono::system_clock::now();
		float time = 0.0f;

		while (time < duration)
		{
			const auto end = std::chrono::system_clock::now();
			const std::chrono::duration<float> elapsedTime = end - start;
			time = elapsedTime.count();

			float x = Random::UniformFloat() * magnitude;
			float y = Random::UniformFloat() * magnitude;

			//auto cam = mCameraService->GetCamera();
			Vector3 newPos = mCameraService->GetCamera().GetPosition() + Vector3(x, y, 0.0f);
			mCameraService->GetCamera().SetPosition(newPos);

			yield_return(nullptr);
		}
	};
}