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

	mStandardEffect.SetBlendState(BlendState::Mode::AlphaBlend);
	mStandardEffect.Initialize(mSampleFilter);
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mStandardEffect.SetDepthBias(0.000021f);
	mStandardEffect.SetBumpWeight(0.25f);
	mStandardEffect.SetSampleSize(0);

	mTexturingEffect.Initialize();
	mSkyBoxEffect.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Clamp);

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
	mGaussianBlurEffect.BlurIterations() = 10;
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
	mNewSkyBox.clear();

	mGaussianBlurEffect.Terminate();
	mPostProccessingEffect.Terminate();

	mBloomRenderTarget.Terminate();
	mBaseRenderTarget.Terminate();
	mScreenQuad.Terminate();

	mShadowEffect.Terminate();
	mTerrainEffect.Terminate();
	mSkyBoxEffect.Terminate();
	mTexturingEffect.Terminate();
	mStandardEffect.Terminate();
}

void RenderService::Update(float deltaTime)
{
	mFPS = static_cast<int>((1.0f / deltaTime) + 0.5f); // TODO: Update FPS less frequently? At least display? Also only when we request it and not just do this math every frame for no reason.

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
	mSkyBoxEffect.SetCamera(camera);
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

	//mBaseRenderTarget.BeginRender();
	{	
		mSkyBoxEffect.Begin();
		if (mSkyDome.diffuseMapId != 0) { mSkyBoxEffect.Render(mSkyDome); }
		if (mSkyBox.diffuseMapId != 0) { mSkyBoxEffect.Render(mSkyBox); }
		mSkyBoxEffect.Render(mNewSkyBox); // TODO:
		mSkyBoxEffect.End();

		mTexturingEffect.Begin();
		for (auto& entry : mBasicMeshRendererEntrys) {
			mTexturingEffect.Render(entry->GetRenderObject());
		}
		for (auto& entry : mBasicRenderEntries) {
			mTexturingEffect.Render(entry.renderGroup);
		}
		mTexturingEffect.End();

		mStandardEffect.Begin();
		for (auto& entry : mRenderEntries) {
			mStandardEffect.Render(entry.renderGroup);
		}
		for (auto& entry : mMeshRendererEntrys)
		{
			if (!entry->GetIsTransparent()) // TODO: This is a hack. Separate these groupings in 2? We should also sort the transparent objects so they are sorted back to front.
			{
				mStandardEffect.Render(entry->GetRenderObject());
			}
		}
		mStandardEffect.End();

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

		std::vector<MeshRendererComponent*> transparentObjects;

		for (auto& entry : mMeshRendererEntrys)
		{
			if (entry->GetIsTransparent())
			{
				transparentObjects.push_back(entry);
			}
		}

		const Vector3& cameraPos = camera.GetPosition();

		// Hack to sort transparent stuff from back to front. 
		std::sort(
			transparentObjects.begin(),
			transparentObjects.end(),
			[&cameraPos](const auto* a, const auto* b)
			{
				const TransformComponent* transformA = a->GetOwner().GetComponent<TransformComponent>();
				const TransformComponent* transformB = b->GetOwner().GetComponent<TransformComponent>();

				const Vector3 posA =
					transformA != nullptr
					? transformA->GetPosition()
					: Vector3::Zero;

				const Vector3 posB =
					transformB != nullptr
					? transformB->GetPosition()
					: Vector3::Zero;

				const float distSqA =
					MagnitudeSqr(posA - cameraPos);

				const float distSqB =
					MagnitudeSqr(posB - cameraPos);

				return distSqA > distSqB;
			});

		mStandardEffect.Begin();
		for (auto* entry : transparentObjects)
		{
			mStandardEffect.Render(entry->GetRenderObject());
		}
		mStandardEffect.End();

		if (mTerrainService) {
			mTerrainEffect.Begin();
			mTerrainEffect.Render(mTerrainService->GetTerrainRenderObject());
			mTerrainEffect.End();
		}
	}
	//mBaseRenderTarget.EndRender();

	//mBloomRenderTarget.BeginRender(); //TODO: Clean up
	//{
	//	mStandardEffect.Begin();
	//	for (auto& entry : mMeshRendererEntrys)
	//	{
	//		Material dummyMaterial;
	//		dummyMaterial.power = 1.0f;
	//		RenderObject& renderObject = entry->GetRenderObject();

	//		bool swap = false;
	//		if (entry->GetShouldBloom() == false)
	//		{
	//			std::swap(renderObject.material, dummyMaterial);
	//			swap = true;
	//		}

	//		mStandardEffect.Render(renderObject);

	//		if (swap)
	//		{
	//			std::swap(renderObject.material, dummyMaterial);
	//		}
	//	}
	//	mStandardEffect.End();
	//}
	//mBloomRenderTarget.EndRender();

	// ------------------------------------------------------------
	//mGaussianBlurEffect.Begin();
	//{
	//	mGaussianBlurEffect.Render(mScreenQuad);
	//}
	//mGaussianBlurEffect.End();

	//mPostProccessingEffect.Begin();
	//{
	//	mPostProccessingEffect.Render(mScreenQuad);
	//}
	//mPostProccessingEffect.End();
	// ------------------------------------------------------------

	// TODO: Grid settings like size, on/off, and y value?
	SimpleDraw::AddTransform(Matrix4::Identity); // TODO: Add settings to display these.
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
	mSkyBoxEffect.DebugUI();
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

	if (ImGui::CollapsingHeader("Sky Box Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{	
		int index = 0;
		for (RenderObject& ro : mNewSkyBox)
		{
			ImGui::PushID(index++);
			ImGui::DragFloat3("Position##RenderService", &ro.transform.position.x, 0.1f);
			ImGui::DragFloat4("Rotation##RenderService", &ro.transform.rotation.w, 0.1f);
			ImGui::DragFloat3("Scale##RenderService", &ro.transform.scale.x, 0.1f);
			ImGui::PopID();
			ImGui::Separator();
		}
	}
}

void RenderService::LoadCubeMapSkyBox(const std::vector<const char*>& fileNames, float size)
{
	if (size < 0.0f) { size = mSkyBoxDefaultSize; }

	mNewSkyBox.clear(); // TODO: Need better clear logic
	const std::vector<Mesh> meshes = MeshBuilder::CreateCubeSkyBox();
	for (auto& mesh : meshes)
	{
		RenderObject ro;
		ro.meshBuffer.Initialize(mesh);
		ro.transform.scale *= size;
		mNewSkyBox.push_back(ro);
	}

	auto tm = TextureManager::Get();
	mNewSkyBox[0].diffuseMapId = tm->LoadTexture(fileNames[0]);
	mNewSkyBox[1].diffuseMapId = tm->LoadTexture(fileNames[1]);
	mNewSkyBox[2].diffuseMapId = tm->LoadTexture(fileNames[2]);
	mNewSkyBox[3].diffuseMapId = tm->LoadTexture(fileNames[3]);
	mNewSkyBox[4].diffuseMapId = tm->LoadTexture(fileNames[4]);
	mNewSkyBox[5].diffuseMapId = tm->LoadTexture(fileNames[5]);
}

void RenderService::LoadCrossCubeMapSkyBox(const char* fileName, float size)
{
	if (size < 0.0f) { size = mSkyBoxDefaultSize; }
	mSkyBox.diffuseMapId = TextureManager::Get()->LoadTexture(fileName);
	mSkyBox.meshBuffer.Initialize(MeshBuilder::CreateCrossCubeSkyBox());
	mSkyBox.transform.scale *= size;
}

void RenderService::LoadSkyDome(const char* fileName, int divisions, float radius)
{
	if (radius < 0.0f) { radius = mSkyBoxDefaultSize; }
	mSkyDome.diffuseMapId = TextureManager::Get()->LoadTexture(fileName);
	mSkyDome.meshBuffer.Initialize(MeshBuilder::CreateSkyDome(divisions, divisions, radius));
}

void RenderService::SetSkyBoxPos(SAGE::Math::Vector3 position)
{
	//mSkyBox.transform.position = position;
}

void RenderService::SetSkyDomePos(SAGE::Math::Vector3 position)
{
	mSkyDome.transform.position = position;
}

void RenderService::SetShadowFocus(const Math::Vector3& focusPosition)
{
	mShadowEffect.SetFocus(focusPosition);
}

RenderGroup* RenderService::Register(const ModelComponent* modelComponent, bool isBasic)
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
	return &entry.renderGroup;
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
