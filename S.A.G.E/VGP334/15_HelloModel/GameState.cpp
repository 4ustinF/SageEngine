#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Graphics::AnimationUtil;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	GraphicsSystem::Get()->SetClearColor(Colors::Black);
	//auto inputSystem = InputSystem::Get();
	//inputSystem->isSnap = true;

	auto tm = TextureManager::Get();

	mCamera.SetPosition({0.0f, 3.5f, -4.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});

	mDirectionalLight.direction = Math::Normalize({1.0f, -1.0f, 1.0f});
	mDirectionalLight.ambient = { 0.0f, 0.0f, 0.0f, 1.0f};
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f};
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mStandardEffect.Initialize();
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mStandardEffect.SetDepthBias(0.000068f);
	mStandardEffect.SetBumpWeight(0.25f);
	mStandardEffect.SetSampleSize(5);

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(mDirectionalLight);
	mShadowEffect.SetSize(10.0f);

	//auto gs = GraphicsSystem::Get();
	//const auto screenWidth = gs->GetBackBufferWidth();
	//const auto screenHeight = gs->GetBackBufferHeight();

	const char* modelFileName = "../../Assets/Models/Maria_J_J_Ong/maria.model";
	ModelIO::LoadModel(modelFileName, mCharacterModel);
	ModelIO::LoadMaterial(modelFileName, mCharacterModel);
	ModelIO::LoadSkeleton(modelFileName, mCharacterModel);
	ModelIO::LoadAnimationSet(modelFileName, mCharacterModel);
	ModelIO::LoadAnimationSet("../../Assets/Models/Maria_J_J_Ong/maria_walk.animset", mCharacterModel);
	ModelIO::LoadAnimationSet("../../Assets/Models/Maria_J_J_Ong/maria_strafe_right.animset", mCharacterModel);
	ModelIO::LoadAnimationSet("../../Assets/Models/Maria_J_J_Ong/maria_retreat.animset", mCharacterModel);
	ModelIO::LoadAnimationSet("../../Assets/Models/Maria_J_J_Ong/maria_strafe_left.animset", mCharacterModel);
	ModelIO::LoadAnimationSet("../../Assets/Models/Maria_J_J_Ong/maria_attack.animset", mCharacterModel);
	ModelIO::LoadAnimationSet("../../Assets/Models/Maria_J_J_Ong/maria_block.animset", mCharacterModel);

	// Model
	//mCharacterAnimator.Initialize(&mCharacterModel);
	//mCharacterAnimator.PlayAnimation(0);

	for (int i = 0; i < mCharacterModel.animationSet.size(); ++i)
	{
		auto anim = mCharacterAnimator.AddAnimator();
		anim->Initialize(&mCharacterModel);
		anim->PlayAnimation(i);
		anim->mLooping = true;
	}

	mCharacter = CreateRenderGroup(mCharacterModel, &mCharacterAnimator);

	// Ground
	mGroundRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mGroundRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	mGroundRenderObject.material.power = 10.0f;
	mGroundRenderObject.diffuseMapId = tm->LoadTexture("concrete.jpg");
	mGroundRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(25, 25, 1.0f));	
}

void GameState::Terminate()
{
	mGroundRenderObject.Terminate();
	CleanUpRenderGroup(mCharacter);
	mShadowEffect.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 2.0f, 10.0f, 0.1f, deltaTime);
	mFPS = 1.0f / deltaTime;

	auto inputSystem = InputSystem::Get();

	// Animation
	//if (inputSystem->IsMousePressed(MouseButton::LBUTTON)) {

	//	for (int i = 1; i < mCharacterModel.animationSet.size() -1; ++i)
	//	{
	//		mCharacterAnimator.SetBlendWeight(i, 0.0f);
	//	}

	//	mCharacterAnimator.SetBlendWeight(mCharacterModel.animationSet.size() - 1, 1.0f);
	//}

	//if (inputSystem->IsMouseUp(MouseButton::LBUTTON)) {
	//	mCharacterAnimator.SetBlendWeight(mCharacterModel.animationSet.size() - 1, 0.0f);
	//}

	if (inputSystem->IsKeyDown(KeyCode::UP)) {
		mSpeedUp += deltaTime;
	}
	else {
		mSpeedUp -= deltaTime;
	}
	mSpeedUp = Clamp(mSpeedUp, 0.0f, 1.0f);
	mCharacterAnimator.SetBlendWeight(1, mSpeedUp);

	if (inputSystem->IsKeyDown(KeyCode::DOWN)) {
		mSpeedDown += deltaTime;
	}
	else {
		mSpeedDown -= deltaTime;
	}
	mSpeedDown = Clamp(mSpeedDown, 0.0f, 1.0f);
	mCharacterAnimator.SetBlendWeight(3, mSpeedDown);

	if (inputSystem->IsKeyDown(KeyCode::RIGHT)) {
		mSpeedRight += deltaTime;
	}
	else {
		mSpeedRight -= deltaTime;
	}
	mSpeedRight = Clamp(mSpeedRight, 0.0f, 1.0f);
	mCharacterAnimator.SetBlendWeight(2, mSpeedRight);

	if (inputSystem->IsKeyDown(KeyCode::LEFT)) {
		mSpeedLeft += deltaTime;
	}
	else {
		mSpeedLeft -= deltaTime;
	}
	mSpeedLeft = Clamp(mSpeedLeft, 0.0f, 1.0f);
	mCharacterAnimator.SetBlendWeight(4, mSpeedLeft);

	//if (mSpeedUp <= 0.0f && mSpeedDown <= 0.0f && mSpeedRight <= 0.0f && mSpeedLeft <= 0.0f)
	//{
	//	idleSpeed += deltaTime;

	//	idleSpeed = Clamp(idleSpeed, 0.0f, 1.0f);
	//	mCharacterAnimator.SetBlendWeight(0, idleSpeed);
	//}
	//else
	//{
	//	idleSpeed -= deltaTime;

	//	idleSpeed = Clamp(idleSpeed, 0.0f, 1.0f);
	//	mCharacterAnimator.SetBlendWeight(0, idleSpeed);
	//}

	//Movement
	for (auto& renderObject : mCharacter)
	{
		if (inputSystem->IsKeyDown(KeyCode::UP)) {
			renderObject.transform.position.z -= mSpeedUp * deltaTime;
		}
		if (inputSystem->IsKeyDown(KeyCode::RIGHT)) {
			renderObject.transform.position.x -= mSpeedRight * deltaTime;
		}
		if (inputSystem->IsKeyDown(KeyCode::DOWN)) {
			renderObject.transform.position.z += mSpeedDown * deltaTime;
		}
		if (inputSystem->IsKeyDown(KeyCode::LEFT)) {
			renderObject.transform.position.x += mSpeedLeft * deltaTime;
		}
	}

	mCharacterAnimator.Update(deltaTime);
}

void GameState::Render()
{
	mShadowEffect.Begin();
	mShadowEffect.Render(mCharacter);
	mShadowEffect.End();

	mStandardEffect.Begin();
	mStandardEffect.Render(mGroundRenderObject);
	if (mDrawSkeleton) {
		std::vector<Matrix4> boneTransforms;
		AnimationUtil::ComputeBoneTransforms(*mCharacterModel.skeleton, boneTransforms, [animator = &mCharacterAnimator](const Bone* bone) {return animator->GetTransform(bone); });
		AnimationUtil::DrawSkeleton(*mCharacterModel.skeleton, boneTransforms);
	}
	else {
		mStandardEffect.Render(mCharacter);
	}
	mStandardEffect.End();

	//SimpleDraw::AddTransform(Math::Matrix4::Identity);
	SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %f", mFPS);
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::DragFloat3("Direction##Light", &mDirectionalLight.direction.x, 0.01f, -1.0f, 1.0f)) {
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}
		
		ImGui::ColorEdit4("Ambient##Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##Light", &mDirectionalLight.specular.r);
	}

	if (ImGui::CollapsingHeader("Ground", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Ground", &mGroundRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Ground", &mGroundRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Ground", &mGroundRenderObject.material.specular.r);
		ImGui::ColorEdit4("Emissive##Ground", &mGroundRenderObject.material.emissive.r);
		ImGui::DragFloat("Power##Ground", &mGroundRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Model", &mCharacter[0].material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Model", &mCharacter[0].material.diffuse.r);
		ImGui::ColorEdit4("Specular##Model", &mCharacter[0].material.specular.r);
		ImGui::ColorEdit4("Emissive##Model", &mCharacter[0].material.emissive.r);
		ImGui::DragFloat("Power##Model", &mCharacter[0].material.power, 1.0f, 1.0f, 100.0f);
	}

	// Includes
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();
	ImGui::Checkbox("Draw Skeleton", &mDrawSkeleton);
	ImGui::End();
}