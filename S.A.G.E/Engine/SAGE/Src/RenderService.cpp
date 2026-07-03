#include "Precompiled.h"
#include "RenderService.h"

#include "CameraService.h"
#include "TerrainService.h"

#include "GameObject.h"
#include "GameWorld.h"

#include "AnimatorComponent.h"
#include "MeshRendererComponent.h"
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

	mPostProccessingEffect.Initialize();
	mPostProccessingEffect.SetMode(PostProcessingEffect::Mode::Combine2);
	mPostProccessingEffect.SetTexture(&mBaseRenderTarget, 0);
	mPostProccessingEffect.SetTexture(&mGaussianBlurEffect.GetResultTexture(), 1);
	mPostProccessingEffect.SetIntensity(25.0f);

	mGaussianBlurEffect.Initialize();
	mGaussianBlurEffect.SetSourceTexture(mBloomRenderTarget);
	mGaussianBlurEffect.BlurIterations() = 10.0f;
	mGaussianBlurEffect.BlurSaturation() = 1.0f;

	mScreenQuad.meshBuffer.Initialize(MeshBuilder::CreateScreenQuad());

	auto gs = GraphicsSystem::Get();
	const auto screenWidth = gs->GetBackBufferWidth();
	const auto screenHeight = gs->GetBackBufferHeight();
	mBaseRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);
	mBloomRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);
}

void RenderService::Terminate()
{
	mGaussianBlurEffect.Terminate();
	mPostProccessingEffect.Terminate();

	mBloomRenderTarget.Terminate();
	mBaseRenderTarget.Terminate();
	mScreenQuad.Terminate();

	mShadowEffect.Terminate();
	mTerrainEffect.Terminate();
	mTexturingEffect.Terminate();
	mStandardEffect.Terminate();
}

void RenderService::Update(float deltaTime)
{
	mFPS = static_cast<int>((1.0f / deltaTime) + 0.5f);

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
			////auto transform = *(static_cast<const Graphics::Transform*>(entry.transformComponent));
			//transform.rotation = rotation * transform.rotation;
			renderObject.transform = entry.transformComponent->GetTransform();
		}
	}

	for (auto& entry : mBasicRenderEntries)
	{
		for (auto& renderObject : entry.renderGroup)
		{
			renderObject.transform = entry.transformComponent->GetTransform();
		}
	}

	mBaseRenderTarget.BeginRender();
	{
		mTexturingEffect.Begin();
		if (mSkyDome.diffuseMapId != 0) { mTexturingEffect.Render(mSkyDome); }
		if (mSkyBox.diffuseMapId != 0) { mTexturingEffect.Render(mSkyBox); }
		for (auto& entry : mBasicRenderEntries) {
			mTexturingEffect.Render(entry.renderGroup);
		}
		for (auto& entry : mBasicMeshRendererEntrys) {
			mTexturingEffect.Render(entry->GetRenderObject());
		}
		mTexturingEffect.End();

		mShadowEffect.Begin();
		for (auto& entry : mRenderEntries) {
			mShadowEffect.Render(entry.renderGroup);
		}
		for (auto& entry : mMeshRendererEntrys) {
			mShadowEffect.Render(entry->GetRenderObject());
		}
		if (mTerrainService) {
			mShadowEffect.Render(mTerrainService->GetTerrainRenderObject());
		}
		mShadowEffect.End();

		mStandardEffect.Begin();
		for (auto& entry : mRenderEntries) {
			mStandardEffect.Render(entry.renderGroup);
		}
		for (auto& entry : mMeshRendererEntrys) {
			mStandardEffect.Render(entry->GetRenderObject());
		}
		mStandardEffect.End();

		if (mTerrainService) {
			mTerrainEffect.Begin();
			mTerrainEffect.Render(mTerrainService->GetTerrainRenderObject());
			mTerrainEffect.End();
		}
	}
	mBaseRenderTarget.EndRender();

	// TODO: Add UV support to standard effect
	// TODO: Add material data to all the models.
	mBloomRenderTarget.BeginRender(); //TODO: Clean up
	{
		mStandardEffect.Begin();
		for (auto& entry : mMeshRendererEntrys) 
		{
			Material dummyMaterial;
			dummyMaterial.power = 1.0f;
			RenderObject& renderObject = entry->GetRenderObject();

			bool swap = false;
			if (TransformComponent* entryTC = entry->GetOwner().GetComponent<TransformComponent>())
			{
				if (entryTC->GetPosition().y <= 0.0f)
				{
					std::swap(renderObject.material, dummyMaterial);
					swap = true;
				}
			}

			mStandardEffect.Render(renderObject);

			if (swap)
			{
				std::swap(renderObject.material, dummyMaterial);
			}
		}
		mStandardEffect.End();
	}
	mBloomRenderTarget.EndRender();

	// ------------------------------------------------------------
	mGaussianBlurEffect.Begin();
	{
		mGaussianBlurEffect.Render(mScreenQuad);
	}
	mGaussianBlurEffect.End();

	mPostProccessingEffect.Begin();
	{
		mPostProccessingEffect.Render(mScreenQuad);
	}
	mPostProccessingEffect.End();
	// ------------------------------------------------------------


	//SimpleDraw::AddTransform(Matrix4::Identity); // TODO: Add settings to display these.
	//SimpleDraw::AddPlane(20, Colors::White); // TODO: Add settings to display these.
	SimpleDraw::Render(camera);
}

void RenderService::DebugUI()
{
	ImGui::Text("FPS: %i", mFPS);
	// TODO: Make it so we can swap mSampleFilter on the fly.

	if (ImGui::CollapsingHeader("Light##RenderServiceLight", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::DragFloat3("Direction##RenderServiceLight", &mDirectionalLight.direction.x, 0.01f, -1.0f, 1.0f)) {
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}

		ImGui::ColorEdit4("Ambient##RenderServiceLight", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##RenderServiceLight", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##RenderServiceLight", &mDirectionalLight.specular.r);
	}

	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mTexturingEffect.DebugUI();
	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();
	mTerrainEffect.DebugUI();
	ImGui::Separator();

	// ---- TODO: Clean up
	ImGui::Separator();
	mPostProccessingEffect.DebugUI();
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Blur Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragInt("Blur Iteration", &mGaussianBlurEffect.BlurIterations(), 1, 1, 100);
		ImGui::DragFloat("Blur Saturation", &mGaussianBlurEffect.BlurSaturation(), 0.001f, 1.0f, 100.0f);
	}

	ImGui::Separator();

	ImGui::Begin("Render Targets", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Base");
	ImGui::Image(mBaseRenderTarget.GetRawData(), { 256, 144 });
	ImGui::Text("Bloom");
	ImGui::Image(mBloomRenderTarget.GetRawData(), { 256, 144 });
	ImGui::Text("Horizontal Blur");
	ImGui::Image(mGaussianBlurEffect.GetHorizontalBlurTexture().GetRawData(), { 256, 144 });
	ImGui::Text("Vertical Blur");
	ImGui::Image(mGaussianBlurEffect.GetVerticalBlurTexture().GetRawData(), { 256, 144 });
	ImGui::End();
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

void RenderService::Register(const ModelComponent* modelComponent, bool isBasic)
{
	Entry& entry = isBasic ? mBasicRenderEntries.emplace_back() : mRenderEntries.emplace_back();
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

void RenderService::Unregister(const ModelComponent* modelComponent, bool isBasic)
{
	std::vector<Entry>& renderEntrys = isBasic ? mBasicRenderEntries : mRenderEntries;
	auto match = [&](const auto& entry) { return entry.modelComponent == modelComponent; };
	auto iter = std::find_if(renderEntrys.begin(), renderEntrys.end(), match);
	if (iter != renderEntrys.end())
	{
		Entry& entry = *iter;
		CleanUpRenderGroup(entry.renderGroup);
		renderEntrys.erase(iter);
		return;
	}
}

void RenderService::RegisterMeshRenderer(MeshRendererComponent* meshRendererComponent, bool isBasic)
{
	std::vector<MeshRendererComponent*>& renderEntrys = isBasic ? mBasicMeshRendererEntrys : mMeshRendererEntrys;
	renderEntrys.push_back(meshRendererComponent);
}

void RenderService::UnregisterMeshRenderer(MeshRendererComponent* meshRendererComponent, bool isBasic)
{
	std::vector<MeshRendererComponent*>& renderEntrys = isBasic ? mBasicMeshRendererEntrys : mMeshRendererEntrys;
	auto match = [&](const auto& entry) { return entry == meshRendererComponent; };
	auto iter = std::find_if(renderEntrys.begin(), renderEntrys.end(), match);
	if (iter != renderEntrys.end())
	{
		const MeshRendererComponent* entry = *iter;
		renderEntrys.erase(iter);
		return;
	}
}
