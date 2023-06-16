#include "Precompiled.h"
#include "RenderService.h"

#include "CameraService.h"
#include "TerrainService.h"

#include "GameObject.h"
#include "GameWorld.h"

#include "AnimatorComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;

void RenderService::Initialize()
{
	GraphicsSystem::Get()->SetClearColor(Colors::Black);
	SetServiceName("Render Service");

	mCameraService = GetWorld().GetService<CameraService>();
	mTerrainService = GetWorld().GetService<TerrainService>();

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mStandardEffect.Initialize(mSampleFilter);
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mStandardEffect.SetDepthBias(0.000021f);
	mStandardEffect.SetBumpWeight(0.25f);
	mStandardEffect.SetSampleSize(0);

	mTexturingEffect.Initialize();

	mTerrainEffect.Initialize();
	mTerrainEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mTerrainEffect.SetDirectionalLight(mDirectionalLight);
	mTerrainEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mTerrainEffect.SetDepthBias(0.000021f);

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(mDirectionalLight);
	//mShadowEffect.SetSize(200);
}

void RenderService::Terminate()
{
	mShadowEffect.Terminate();
	mTerrainEffect.Terminate();
	mTexturingEffect.Terminate();
	mStandardEffect.Terminate();
}

void RenderService::Update(float deltaTime)
{
	mFPS = 1.0f / deltaTime;

	if (mSkyDome.diffuseMapId != 0)
	{
		SetSkyDomePos(mCameraService->GetCamera().GetPosition());
	}

	if (mSkyBox.diffuseMapId != 0)
	{
		SetSkyBoxPos(mCameraService->GetCamera().GetPosition());
	}
}

void RenderService::Render()
{
	auto& camera = mCameraService->GetCamera();
	mStandardEffect.SetCamera(camera);
	mTexturingEffect.SetCamera(camera);
	mTerrainEffect.SetCamera(camera);

	for (auto& entry : mRenderEntries) 
	{
		for (auto& renderObject : entry.renderGroup)
		{
			//const auto angles = entry.modelComponent->GetRotation();
			//const auto rotation = Math::Quaternion::RotationEuler(angles);
			auto transform = *(static_cast<const Graphics::Transform*>(entry.transformComponent));
			//transform.rotation = rotation * transform.rotation;
			renderObject.transform = transform;
		}
	}

	if (mSkyDome.diffuseMapId != 0)
	{
		mTexturingEffect.Begin();
		mTexturingEffect.Render(mSkyDome);
		mTexturingEffect.End();
	}

	if (mSkyBox.diffuseMapId != 0)
	{
		mTexturingEffect.Begin();
		mTexturingEffect.Render(mSkyBox);
		mTexturingEffect.End();
	}

	mShadowEffect.Begin();
	for (auto& entry : mRenderEntries) {
		mShadowEffect.Render(entry.renderGroup);
	}
	if (mTerrainService) {
		mShadowEffect.Render(mTerrainService->GetTerrainRenderObject());
	}
	mShadowEffect.End();

	mStandardEffect.Begin();
	for (auto& entry : mRenderEntries) {
		mStandardEffect.Render(entry.renderGroup);
	}
	mStandardEffect.End();

	if (mTerrainService) {
		mTerrainEffect.Begin();
		mTerrainEffect.Render(mTerrainService->GetTerrainRenderObject());
		mTerrainEffect.End();
	}

	SimpleDraw::AddTransform(Matrix4::Identity);
	SimpleDraw::AddPlane(20, Colors::White);
	SimpleDraw::Render(camera);
}

void RenderService::DebugUI()
{
	ImGui::Text("FPS: %f", mFPS);

	if (ImGui::CollapsingHeader("Light##RenderServiceLight", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::DragFloat3("Direction##RenderServiceLight", &mDirectionalLight.direction.x, 0.01f, -1.0f, 1.0f)) {
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}

		ImGui::ColorEdit4("Ambient##RenderServiceLight", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##RenderServiceLight", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##RenderServiceLight", &mDirectionalLight.specular.r);
	}
}

void RenderService::LoadSkyDome(const char* fileName)
{
	auto tm = TextureManager::Get();
	mSkyDome.diffuseMapId = tm->LoadTexture(fileName);
	mSkyDome.meshBuffer.Initialize(MeshBuilder::CreateSkyDome(256, 256, 500.0f));
}

void RenderService::LoadSkyBox(const char* fileName)
{
	auto tm = TextureManager::Get();
	mSkyBox.diffuseMapId = tm->LoadTexture(fileName);
	mSkyBox.meshBuffer.Initialize(MeshBuilder::CreateSkyBox());
	mSkyBox.transform.scale *= 500.0f;
}

void RenderService::SetSkyDomePos(SAGE::Math::Vector3 position)
{
	mSkyDome.transform.position = position;
}

void RenderService::SetSkyBoxPos(SAGE::Math::Vector3 position)
{
	mSkyBox.transform.position = position;
}

void RenderService::SetShadowFocus(const Math::Vector3& focusPosition)
{
	mShadowEffect.SetFocus(focusPosition);
}

void RenderService::Register(const ModelComponent* modelComponent)
{
	Entry& entry = mRenderEntries.emplace_back();
	auto& gameObject = modelComponent->GetOwner();

	entry.animatorComponent = gameObject.GetComponent<AnimatorComponent>();
	entry.modelComponent = modelComponent;
	entry.transformComponent = gameObject.GetComponent<TransformComponent>();

	const IAnimator* animator = nullptr;
	if (entry.animatorComponent) {
		animator = &entry.animatorComponent->GetAnimator();
	}

	entry.renderGroup = CreateRenderGroup(modelComponent->GetModel(), animator);
}

void RenderService::Unregister(const ModelComponent* modelComponent)
{
	auto match = [&](const auto& entry) {return entry.modelComponent == modelComponent; };
	auto iter = std::find_if(mRenderEntries.begin(), mRenderEntries.end(), match);
	if (iter != mRenderEntries.end())
	{
		Entry& entry = *iter;
		CleanUpRenderGroup(entry.renderGroup);
		mRenderEntries.erase(iter);
	}
}