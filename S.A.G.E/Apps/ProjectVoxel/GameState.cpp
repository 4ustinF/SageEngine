#include "GameState.h"
#include "CloudService.h"
#include "PlayerUIService.h"
#include "WorldService.h"
#include "MobService.h"

#include "AnimalComponent.h"
#include "PlayerControllerComponent.h"
#include "CameraControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

namespace
{
	// Check for custom components
	bool OnMake(const char* componentName, const rapidjson::Value& value, GameObject& gameObject)
	{
		if (strcmp(componentName, "PlayerControllerComponent") == 0)
		{
			auto playerControllerComponent = gameObject.AddComponent<PlayerControllerComponent>();
			return true;
		}
		else if (strcmp(componentName, "CameraControllerComponent") == 0)
		{
			auto cameraControllerComponent = gameObject.AddComponent<CameraControllerComponent>();
			if (value.HasMember("NormSpeed")) {
				const auto speed = value["NormSpeed"].GetFloat();
				cameraControllerComponent->SetNormSpeed(speed);
			}
			if (value.HasMember("FastSpeed")) {
				const auto speed = value["FastSpeed"].GetFloat();
				cameraControllerComponent->SetFastSpeed(speed);
			}
			if (value.HasMember("TurnSpeed")) {
				const auto speed = value["TurnSpeed"].GetFloat();
				cameraControllerComponent->SetTurnSpeed(speed);
			}
			if (value.HasMember("Distance")) {
				const auto distance = value["Distance"].GetFloat();
				cameraControllerComponent->SetDistance(distance);
			}
			if (value.HasMember("CameraElevation")) {
				const auto elevation = value["CameraElevation"].GetFloat();
				cameraControllerComponent->SetCameraElevation(elevation);
			}
			if (value.HasMember("LookElevation")) {
				const auto elevation = value["LookElevation"].GetFloat();
				cameraControllerComponent->SetLookElevation(elevation);
			}
			return true;
		}
		else if (strcmp(componentName, "AnimalComponent") == 0)
		{
			auto animalComponent = gameObject.AddComponent<AnimalComponent>();
			return true;
		}
		return false;
	}
}

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>()->SetSampleFilter(Sampler::Filter::Point);
	mGameWorld.AddService<CloudService>();
	mGameWorld.AddService<BPhysicsService>();
	mGameWorld.AddService<MobService>();
	mGameWorld.AddService<WorldService>();
	mGameWorld.AddService<ParticleService>();
	mGameWorld.AddService<PlayerUIService>();
	//mGameWorld.AddService<PostProcessingService>();
	mGameWorld.Initialize(100);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/voxel_level.json");

	mGameWorld.GetService<RenderService>()->GetDirectionalLight().ambient = { 0.4f, 0.4f, 0.4f, 1.0f };

	mStandardEffect.SetBlendState(BlendState::Mode::AlphaBlend);
	mStandardEffect.Initialize(Sampler::Filter::Point);
	mStandardEffect.SetCamera(mGameWorld.GetService<CameraService>()->GetCamera());
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mGameWorld.GetService<RenderService>()->GetDirectionalLight());
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mStandardEffect.UseBumpMap(false);
	mStandardEffect.SetDepthBias(0.000040f);
	mStandardEffect.SetBumpWeight(0.0f);
	mStandardEffect.SetSampleSize(0);
	//mStandardEffect.UseFog(true);
	//mStandardEffect.SetFogColor(Colors::Gray);
	//mStandardEffect.SetFogStart(64.0f);
	//mStandardEffect.SetFogEnd(120.0f);

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(mGameWorld.GetService<RenderService>()->GetDirectionalLight());
	mShadowEffect.SetSize(200.0f);

	////const char* modelFileName = "../../Assets/Models/Max/Max.model";
	//const char* modelFileName = "../../Assets/Models/Max/PlayerModel.model";
	//ModelIO::LoadModel(modelFileName, mCharacterModel);
	//ModelIO::LoadMaterial(modelFileName, mCharacterModel);
	//ModelIO::LoadSkeleton(modelFileName, mCharacterModel);
	//ModelIO::LoadAnimationSet("../../Assets/Models/Max/PlayerModel_Walk.animset", mCharacterModel);
	//ModelIO::LoadAnimationSet("../../Assets/Models/Max/PlayerModel_Walk.animset", mCharacterModel);

	//for (int i = 0; i < mCharacterModel.animationSet.size(); ++i)
	//{
	//	auto anim = mCharacterAnimator.AddAnimator();
	//	anim->Initialize(&mCharacterModel);
	//	anim->PlayAnimation(i);
	//	anim->mLooping = true;
	//}

	//mCharacter = CreateRenderGroup(mCharacterModel, &mCharacterAnimator);
	//for (auto& mesh : mCharacter) { mesh.transform.position.y = 25.0f; }

	////  0 - Head | 1 - Body | 2 - Right Arm | 3 - Left Arm | 4 - Right Leg | 5 - Left Leg 
	//for (int i = 0; i < mCharacterModel.meshData.size(); ++i)
	//{
	//	RenderObject renderObject;
	//	renderObject.material.ambient = mCharacterModel.materialData[0].material.ambient;
	//	renderObject.material.diffuse = mCharacterModel.materialData[0].material.diffuse;
	//	renderObject.material.specular = mCharacterModel.materialData[0].material.specular;
	//	renderObject.material.emissive = mCharacterModel.materialData[0].material.emissive;
	//	renderObject.material.power = mCharacterModel.materialData[0].material.power;
	//	renderObject.diffuseMapId = TextureManager::Get()->LoadTexture(mCharacterModel.materialData[0].diffuseMapName);
	//	renderObject.meshBuffer.Initialize(mCharacterModel.meshData[i].mesh);
	//	renderObject.transform.position.y = 25.0f;
	//	mCharacter.push_back(std::move(renderObject));
	//}

	mBgMusicID = SoundEffectManager::Get()->Load("C418_Sweden_(Naz3nt_Remix).wav");
	SoundEffectManager::Get()->Play(mBgMusicID, true);

	mGameWorld.GetService<WorldService>()->UpdateAABBS();

	mBlockInHandRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mBlockInHandRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mBlockInHandRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mBlockInHandRenderObject.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	mBlockInHandRenderObject.material.power = 10.0f;
	mBlockInHandRenderObject.diffuseMapId = TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/dirt.png");
	mBlockInHandRenderObject.meshBuffer.Initialize(MeshBuilder::CreateCube());
	mBlockInHandRenderObject.transform.scale = Vector3(0.3f);

	mItemInHandRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mItemInHandRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mItemInHandRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mItemInHandRenderObject.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	mItemInHandRenderObject.material.power = 10.0f;
	//mItemInHandRenderObject.diffuseMapId = TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_sword.png");

	Mesh itemInHandMesh;
	const float itemSize = 0.3f;
	itemInHandMesh.vertices.push_back({ {-itemSize, -itemSize, +itemSize}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, 1.0f} });
	itemInHandMesh.vertices.push_back({ {-itemSize, +itemSize, +itemSize}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.0f, 0.0f} });
	itemInHandMesh.vertices.push_back({ {+itemSize, +itemSize, +itemSize}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {1.0f, 0.0f} });
	itemInHandMesh.vertices.push_back({ {+itemSize, -itemSize, +itemSize}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {1.0f, 1.0f} });

	itemInHandMesh.indices = { 2, 1, 0,	2, 0, 3 };
	mItemInHandRenderObject.meshBuffer.Initialize(std::move(itemInHandMesh));
	mItemInHandRotation = Quaternion::RotationAxis(Vector3::YAxis, 110.0f * Constants::DegToRad) * Quaternion::RotationAxis(Vector3::ZAxis, 25.0f * Constants::DegToRad) * Quaternion::RotationAxis(Vector3::XAxis, -15.0f * Constants::DegToRad);

	CreateArm();
}

void GameState::Terminate()
{
	mArmRenderObject.Terminate();
	mItemInHandRenderObject.Terminate();
	//CleanUpRenderGroup(mCharacter);
	mShadowEffect.Terminate();
	mStandardEffect.Terminate();
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);
	auto& cam = mGameWorld.GetService<CameraService>()->GetCamera();
	const Vector3& camPos = cam.GetPosition();

	auto worldService = mGameWorld.GetService<WorldService>();
	worldService->mNewWorldCoords = { (static_cast<int>(camPos.x) + (camPos.x > 0.0f ? 8 : -8)) / 16, (static_cast<int>(camPos.z) + (camPos.z > 0.0f ? 8 : -8)) / 16 };

	// Moved Chunks
	if (worldService->mOldWorldCoords != worldService->mNewWorldCoords)
	{
		// Update colliders
		worldService->UpdateAABBS();

		mShadowEffect.SetFocus(camPos);
	}

	//mCharacterAnimator.Update(deltaTime);

	// Move mArmRenderObject
	mArmRenderObject.transform.rotation = Quaternion::RotationLook(-cam.GetDirection()) * mArmRotation;
	mArmRenderObject.transform.position = GetTranslation(Matrix4::Translation({ 0.65f, -0.5f, 1.05f }) * cam.GetWorldMatrix());
}

void GameState::Render()
{
	//auto pp = mGameWorld.GetService<PostProcessingService>();
	//pp->GetRenderTarget().BeginRender();

	mStandardEffect.Begin();
	{
		//mStandardEffect.Render(mCharacter);
		//std::vector<Matrix4> boneTransforms;
		//AnimationUtil::ComputeBoneTransforms(*mCharacterModel.skeleton, boneTransforms, [animator = &mCharacterAnimator](const Bone* bone) {return animator->GetTransform(bone); });
		//AnimationUtil::DrawSkeleton(*mCharacterModel.skeleton, boneTransforms);
		mStandardEffect.Render(mArmRenderObject);
	}
	mStandardEffect.End();

	//mShadowEffect.Begin();
	//{
	//	//mShadowEffect.Render(mCharacter);
	//}
	//mShadowEffect.End();

	mGameWorld.Render();

	mStandardEffect.Begin();
	{
		const auto& playerUIService = mGameWorld.GetService<PlayerUIService>();
		const auto itemInHand = playerUIService->playerInventory[playerUIService->GetIconSelected()].itemType;
		
		if (IsItemBlock(itemInHand))
		{
			auto& cam = mGameWorld.GetService<CameraService>()->GetCamera();
			mBlockInHandRenderObject.transform.rotation = Quaternion::RotationLook(-cam.GetDirection()) * mArmRotation;
			mBlockInHandRenderObject.transform.position = GetTranslation(Matrix4::Translation({ 0.75f, -0.25f, 1.4f }) * cam.GetWorldMatrix());

			mBlockInHandRenderObject.diffuseMapId = playerUIService->mItemTextureIds.at(itemInHand);
			mStandardEffect.Render(mBlockInHandRenderObject);
		}
		else if (itemInHand != ItemTypes::None)
		{
			auto& cam = mGameWorld.GetService<CameraService>()->GetCamera();
			mItemInHandRenderObject.transform.rotation = Quaternion::RotationLook(-cam.GetDirection()) * mItemInHandRotation;
			mItemInHandRenderObject.transform.position = GetTranslation(Matrix4::Translation({ 1.1f, -0.18f, 1.1f }) * cam.GetWorldMatrix());

			mItemInHandRenderObject.diffuseMapId = playerUIService->mItemTextureIds.at(itemInHand);
			mStandardEffect.Render(mItemInHandRenderObject);
		}

		auto player = mGameWorld.FindGameObject("Player")->GetComponent<PlayerControllerComponent>();
		if (player->IsMining())
		{
			mStandardEffect.Render(player->mMiningCube);
		}
	}
	mStandardEffect.End();

	SimpleDraw::Render(mGameWorld.GetService<CameraService>()->GetCamera());
	//pp->GetRenderTarget().EndRender();
	//pp->NewRender();
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();

	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();
	ImGui::End();
}

void GameState::CreateArm()
{
	mArmRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mArmRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mArmRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mArmRenderObject.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	mArmRenderObject.material.power = 10.0f;
	mArmRenderObject.diffuseMapId = TextureManager::Get()->LoadTexture("../Sprites/SageCraft/ArmTexture.png");
	//mArmRenderObject.meshBuffer.Initialize(MeshBuilder::CreateCube());

	//24 unique vertices
	Mesh armMesh;
	const float size = 0.5f;

	//Front - Done
	armMesh.vertices.push_back({ {-size, -size, +size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.25f, 0.25f} });	//0 - FrontBotLeft
	armMesh.vertices.push_back({ {-size, +size, +size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.25f, 0.00f} });	//1 - FrontTopLeft
	armMesh.vertices.push_back({ {+size, +size, +size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.50f, 0.00f} });	//2 - FrontTopRight
	armMesh.vertices.push_back({ {+size, -size, +size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.50f, 0.25f} });	//3 - FrontBotRight

	//Back - Done
	armMesh.vertices.push_back({ {-size, -size, -size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.50f, 0.25f} });	//4 - BackBotLeft
	armMesh.vertices.push_back({ {-size, +size, -size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.50f, 0.00f} });	//5 - BackTopLeft
	armMesh.vertices.push_back({ {+size, +size, -size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.75f, 0.00f} });	//6 - BackTopRight
	armMesh.vertices.push_back({ {+size, -size, -size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, {0.75f, 0.25f} });	//7 - BackBotRight

	//Left - Done
	armMesh.vertices.push_back({ {+size, -size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.25f, 1.00f} });	//8  - LeftBotLeft
	armMesh.vertices.push_back({ {+size, +size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.00f, 1.00f} });	//9  - LeftTopLeft
	armMesh.vertices.push_back({ {+size, +size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.00f, 0.25f} });	//10 - LeftTopRight
	armMesh.vertices.push_back({ {+size, -size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.25f, 0.25f} });	//11 - LeftBotRight

	//Right - Done
	armMesh.vertices.push_back({ {-size, -size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.50f, 1.0f} });	//12 - RightBotLeft
	armMesh.vertices.push_back({ {-size, +size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.75f, 1.0f} });	//13 - RightTopLeft
	armMesh.vertices.push_back({ {-size, +size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.75f, 0.25f} });	//14 - RightTopRight
	armMesh.vertices.push_back({ {-size, -size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, {0.50f, 0.25f} });	//15 - RightBotRight

	//Top - Done
	armMesh.vertices.push_back({ {+size, +size, +size}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.50f, 0.25f} });	//16 - TopBotLeft
	armMesh.vertices.push_back({ {+size, +size, -size}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.50f, 1.00f} });	//17 - TopTopLeft
	armMesh.vertices.push_back({ {-size, +size, -size}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.25f, 1.00f} });	//18 - TopTopRight
	armMesh.vertices.push_back({ {-size, +size, +size}, Math::Vector3::YAxis, Math::Vector3::XAxis, {0.25f, 0.25f} });	//19 - TopBotRight

	//Bot - Done
	armMesh.vertices.push_back({ {+size, -size, +size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, {0.75f, 0.25f} });	//20 - BotBotLeft
	armMesh.vertices.push_back({ {+size, -size, -size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, {0.75f, 1.00f} });	//21 - BotTopLeft
	armMesh.vertices.push_back({ {-size, -size, -size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, {1.00f, 1.00f} });	//22 - BotTopRight
	armMesh.vertices.push_back({ {-size, -size, +size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, {1.00f, 0.25f} });	//23 - BotBotRight

	armMesh.indices = { 2, 1, 0,	2, 0, 3,	//Front
					4, 5, 6,	7, 4, 6,	//Back
					8, 9, 10,	11, 8,10,	//Left  
					14,13,12,	14,12,15,	//Right 
					16,17,18,	19,16,18,	//Top
					22,21,20,	22,20,23,	//Bot
	};
	mArmRenderObject.meshBuffer.Initialize(std::move(armMesh));
	mArmRotation = Quaternion::RotationAxis(Vector3::YAxis, 20.0f * Constants::DegToRad) * Quaternion::RotationAxis(Vector3::ZAxis, 25.0f * Constants::DegToRad) * Quaternion::RotationAxis(Vector3::XAxis, 15.0f * Constants::DegToRad);

	mArmRenderObject.transform.scale = { 0.25f, 0.25f , 1.0f };
}
