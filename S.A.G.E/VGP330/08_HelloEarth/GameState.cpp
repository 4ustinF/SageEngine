#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({0.0f, 3.0f, -3.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});

	mDirectionalLight.direction = Math::Normalize({1.0f, -1.0f, 1.0f});
	mDirectionalLight.ambient = { 0.0f, 0.0f, 0.0f, 1.0f};
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f};
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/EarthShader.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/EarthShader.fx");

	//mTransformBuffer.Initialize();
	//mLightBuffer.Initialize();
	//mMaterialBuffer.Initialize();
	//mSettingsBuffer.Initialize();

	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);

}

void GameState::Terminate()
{
	mSampler.Terminate();

	mSettingsBuffer.Terminate();
	mTransformBuffer.Terminate();
	mMaterialBuffer.Terminate();
	mDirectionalLight.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void GameState::Update(float deltaTime)
{
	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 1.0f, 10.0f, 0.1f, deltaTime);

	mFPS = 1.0f / deltaTime;
}

void GameState::Render()
{
	SimpleDraw::AddPlane(20, Colors::White);
	SimpleDraw::AddTransform(Math::Matrix4::Identity);

	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mLightBuffer.BindVS(1);
	mLightBuffer.BindPS(1);

	mMaterialBuffer.BindPS(2);
	mSettingsBuffer.BindVS(3);
	mSettingsBuffer.BindPS(3);

	mSampler.BindVS(0);
	mSampler.BindPS(0);

	//const auto& matWorld = renderObject.transform.GetMatrix4();
	//const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mCamera.GetViewMatrix();
	const auto& proj = mCamera.GetProjectionMatrix();

	TransformData transformData;
	//transformData.world = Transpose(matWorld);
	//transformData.wvp = Transpose(matWorld * view * proj);
	transformData.viewPosition = mCamera.GetPosition();
	transformData.bumpWeight = mBumpWeight;
	mTransformBuffer.Update(transformData);

	mLightBuffer.Update(*mDirectionalLight);
	//mMaterialBuffer.Update(renderObject.material);
	mSettingsBuffer.Update(mSettingsData);

	//renderObject.diffuseMap.BindPS(0);
	//renderObject.specularMap.BindPS(1);
	//renderObject.bumpMap.BindVS(2);
	//renderObject.normalMap.BindPS(3);

	//renderObject.meshBuffer.Render();

	//Show nomal lines
	if (mShowNormals) {
		for (const auto& v : mMesh.vertices) {
			SimpleDraw::AddLine(v.position, v.position + v.normal, Colors::Cyan);
		}
	}

	SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %f", mFPS);
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Direction##Light", &mDirectionalLight.direction.x, 0.01f, -1.0f, 1.0f)) {
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}
		
		ImGui::ColorEdit4("Ambient##Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##Light", &mDirectionalLight.specular.r);
	}

	if (ImGui::CollapsingHeader("Earth", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//ImGui::ColorEdit4("Ambient##Earth", &mEarthRenderObject.material.ambient.r);
		//ImGui::ColorEdit4("Diffuse##Earth", &mEarthRenderObject.material.diffuse.r);
		//ImGui::ColorEdit4("Specular##Earth", &mEarthRenderObject.material.specular.r);
		//ImGui::DragFloat("Power##Earth", &mEarthRenderObject.material.power, 1.0f, 1.0f, 100.0f);

		//ImGui::DragFloat3("Position", &mEarthRenderObject.transform.position.x, 0.01f);
		//ImGui::DragFloat3("Rotation", &mEarthRenderObject.transform.rotation.x, 0.01f);
	}

	ImGui::Separator();
	ImGui::Checkbox("Show Normals", &mShowNormals);
	ImGui::End();
}