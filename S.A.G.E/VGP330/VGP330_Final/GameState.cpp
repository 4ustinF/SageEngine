#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	auto inputSystem = InputSystem::Get();
	auto tm = TextureManager::Get();
	//inputSystem->isSnap = true;

	mCamera.SetPosition({0.0f, 2.0f, -7.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});
	mCamera.SetFarPlane(100000);

	mDirectionalLight.direction = Math::Normalize({1.0f, -1.0f, 1.0f});
	mDirectionalLight.ambient = { 0.0f, 0.0f, 0.0f, 1.0f};
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f};
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mStandardEffect.Initialize();
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mStandardEffect.SetDepthBias(0.000023f);

	mCrossHatchEffect.Initialize();
	mCrossHatchEffect.SetCamera(mCamera);
	mCrossHatchEffect.SetDirectionalLight(mDirectionalLight);
	mCrossHatchEffect.SetHatchMapDark("hatchDarkest.png");
	mCrossHatchEffect.SetHatchMapBright("hatchbrightest.png");
	mCrossHatchEffect.SetIntensityMultiplier(4.0f);
	mCrossHatchEffect.UseSpecularMap(false);

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(mDirectionalLight);
	//mShadowEffect.SetSize(6.0f);

	mHologramEffect.Initialize();
	mHologramEffect.SetCamera(mCamera);
	mHologramEffect.SetDirectionalLight(mDirectionalLight);
	mHologramEffect.UseBars(false);
	mHologramEffect.SetMainColor(Colors::White);
	mHologramEffect.SetRimColor(Colors::SteelBlue);
	
	auto gs = GraphicsSystem::Get();
	const auto screenWidth = gs->GetBackBufferWidth();
	const auto screenHeight = gs->GetBackBufferHeight();

#pragma region --Master Chief--

	Model masterChiefModel;
	ModelIO::LoadModel("../../Assets/Models/MasterChief/Master_Chief.model", masterChiefModel);

	//// Master Chief Body
	//mMasterChiefBody.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	//mMasterChiefBody.material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	//mMasterChiefBody.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	//mMasterChiefBody.material.power = 10.0f;
	//mMasterChiefBody.diffuseMapId = tm->LoadTexture("../Models/MasterChief/Chief_BaseColor.jpeg");
	//mMasterChiefBody.normalMapId = tm->LoadTexture("../Models/MasterChief/fortnitechiefnormal_addedetails11.jpeg");
	//mMasterChiefBody.meshBuffer.Initialize(masterChiefModel.meshData[0].mesh);

	//// Master Chief Head
	//mMasterChiefHead.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	//mMasterChiefHead.material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	//mMasterChiefHead.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	//mMasterChiefHead.material.power = 10.0f;
	//mMasterChiefHead.diffuseMapId = tm->LoadTexture("../Models/MasterChief/Chief_Fortnite_Helmet_BaseColor.jpeg");
	//mMasterChiefHead.normalMapId = tm->LoadTexture("../Models/MasterChief/Chief_Fortnite_Helmet_Normal.jpeg");
	//mMasterChiefHead.meshBuffer.Initialize(masterChiefModel.meshData[1].mesh);

	const char* modelFileName = "../../Assets/Models/MasterChief/Master_Chief.model";
	ModelIO::LoadModel(modelFileName, mMasterChiefModel);
	ModelIO::LoadMaterial(modelFileName, mMasterChiefModel);
	ModelIO::LoadSkeleton(modelFileName, mMasterChiefModel);

	// Model
	mMasterChief = CreateRenderGroup(mMasterChiefModel);
	for (auto& b : mMasterChief)
	{
		b.transform.rotation = Quaternion::RotationEuler(Vector3(90.0f, 0.0f, 0.0f));
		b.transform.position = Vector3(0.0f, 0.0f, 1.0f);
	}

	// Movement
	float masterChiefMove = -2.0f;
	//mMasterChiefBody.transform.position.x = masterChiefMove;
	//mMasterChiefHead.transform.position.x = masterChiefMove;

#pragma endregion

#pragma region --Cortana--

	Model cortanaModel;
	ModelIO::LoadModel("../../Assets/Models/Cortana/new_cortana_halo4_og.model", cortanaModel);

	Vector3 cortPos = Vector3(0.0f, 0.0f, 0.0f);

	// Cortana Body
	mCortanaBody.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCortanaBody.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mCortanaBody.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCortanaBody.material.power = 10.0f;
	mCortanaBody.diffuseMapId = tm->LoadTexture("../Models/Cortana/1storm_cortana_default_body_diff.png");
	mCortanaBody.normalMapId = tm->LoadTexture("../Models/Cortana/storm_cortana_default_body_normal.png");
	mCortanaBody.meshBuffer.Initialize(cortanaModel.meshData[0].mesh);
	mCortanaBody.transform.position = cortPos;

	// Cortana Head
	mCortanaHead.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCortanaHead.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mCortanaHead.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCortanaHead.material.power = 10.0f;
	mCortanaHead.diffuseMapId = tm->LoadTexture("../Models/Cortana/1storm_cortana_default_head_diff.png");
	mCortanaHead.normalMapId = tm->LoadTexture("../Models/Cortana/storm_cortana_default_head_normal.png");
	mCortanaHead.meshBuffer.Initialize(cortanaModel.meshData[1].mesh);
	mCortanaHead.transform.position = cortPos;

	// Cortana Eyes
	mCortanaEyes.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCortanaEyes.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mCortanaEyes.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCortanaEyes.material.power = 10.0f;
	mCortanaEyes.diffuseMapId = tm->LoadTexture("../Models/Cortana/storm_cortana_default_eye_diff.png");
	mCortanaEyes.normalMapId = tm->LoadTexture("../Models/Cortana/storm_cortana_default_eye_iris_normal.png");
	mCortanaEyes.meshBuffer.Initialize(cortanaModel.meshData[2].mesh);
	mCortanaEyes.transform.position = cortPos;

	// Cortana Hair
	mCortanaHair.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCortanaHair.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mCortanaHair.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCortanaHair.material.power = 10.0f;
	mCortanaHair.diffuseMapId = tm->LoadTexture("../Models/Cortana/1storm_cortana_default_hair_diff.png");
	mCortanaHair.normalMapId = tm->LoadTexture("../Models/Cortana/storm_cortana_default_hair_normal.png");
	mCortanaHair.meshBuffer.Initialize(cortanaModel.meshData[3].mesh);
	mCortanaHair.transform.position = cortPos;

#pragma endregion

	// Ground
	mGroundRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mGroundRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.power = 10.0f;
	mGroundRenderObject.diffuseMapId = tm->LoadTexture("concrete.jpg");
	mGroundRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(20, 20, 1.0f));	

	// Skybox
	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
	mVertexShader.Initialize<VertexPX>(L"../../Assets/Shaders/DoTexturing.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/DoTexturing.fx");
	mConstantBuffer.Initialize(sizeof(Matrix4));

	mSkyBoxTexture.Initialize("../../Assets/Images/starSkybox.png");
	mSkyBoxMesh = MeshBuilder::CreateSkyBoxPX();
	mSkyBoxBuffer.Initialize(mSkyBoxMesh);

	// Bloom
	mBaseRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);
	mBloomRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);

	mPostProccessingEffect.Initialize();
	mPostProccessingEffect.SetMode(PostProcessingEffect::Mode::Combine2);
	mPostProccessingEffect.SetTexture(&mBaseRenderTarget, 0);
	mPostProccessingEffect.SetTexture(&mGaussianBlurEffect.GetResultTexture(), 1);
	mPostProccessingEffect.SetIntensity(25.0f);

	mGaussianBlurEffect.Initialize();
	mGaussianBlurEffect.SetSourceTexture(mBloomRenderTarget);
	mGaussianBlurEffect.BlurIterations() = 1;
	mGaussianBlurEffect.BlurSaturation() = 1;

	mScreenQuad.meshBuffer.Initialize(MeshBuilder::CreateScreenQuad());
}

void GameState::Terminate()
{
	mBloomRenderTarget.Terminate();
	mBaseRenderTarget.Terminate();

	mGaussianBlurEffect.Terminate();
	mPostProccessingEffect.Terminate();

	mSkyBoxBuffer.Terminate();
	mSkyBoxTexture.Terminate();
	mConstantBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mSampler.Terminate();

	mGroundRenderObject.Terminate();
	mCortanaHair.Terminate();
	mCortanaEyes.Terminate();
	mCortanaHead.Terminate();
	mCortanaBody.Terminate();
	//mMasterChiefHead.Terminate();
	//mMasterChiefBody.Terminate();
	CleanUpRenderGroup(mMasterChief);

	mHologramEffect.Terminate();
	mShadowEffect.Terminate();
	mCrossHatchEffect.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 0.5f, 8.0f, 0.1f, deltaTime);
	mFPS = 1.0f / deltaTime;

	auto inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::SPACE)) {
		mPause = !mPause;
	}

	const float dt = mPause ? 0.0f : deltaTime;
	mRunningTime += dt;
}

void GameState::Render()
{
	mBaseRenderTarget.BeginRender();
	{
		mShadowEffect.Begin();
		//mShadowEffect.Render(mMasterChiefHead);
		//mShadowEffect.Render(mMasterChiefBody);
		mShadowEffect.Render(mMasterChief);
		mShadowEffect.End();

		mStandardEffect.Begin();
		mStandardEffect.Render(mGroundRenderObject);
		//mStandardEffect.Render(mMasterChiefHead);
		//mStandardEffect.Render(mMasterChiefBody);
		//mStandardEffect.Render(mMasterChief);
		mStandardEffect.End();

		mCrossHatchEffect.Begin();
		mCrossHatchEffect.Render(mMasterChief);
		mCrossHatchEffect.End();

		mHologramEffect.Begin();
		mHologramEffect.Render(mCortanaHair);
		mHologramEffect.Render(mCortanaEyes);
		mHologramEffect.Render(mCortanaHead);
		mHologramEffect.Render(mCortanaBody);
		mHologramEffect.End();

		//SkyBox
		mVertexShader.Bind();
		mPixelShader.Bind();
		mSampler.BindPS(0);
		mConstantBuffer.BindVS(0);

		Matrix4 view = mCamera.GetViewMatrix();
		Matrix4 proj = mCamera.GetProjectionMatrix();

		const float skyboxSize = 10000.0f;
		Matrix4 skyBoxWorld = Matrix4::Identity * Matrix4::Scaling({ skyboxSize, skyboxSize, skyboxSize }) * Matrix4::RotationY(Math::Constants::DegToRad * 60);
		Matrix4 SkyBoxWVP = Transpose(Math::Matrix4::Identity * skyBoxWorld * view * proj);
		mConstantBuffer.Update(&SkyBoxWVP);
		mSkyBoxTexture.BindPS(0);
		mSkyBoxBuffer.Render();

		SimpleDraw::Render(mCamera);
	}
	mBaseRenderTarget.EndRender();

	mBloomRenderTarget.BeginRender();
	{
		//Swap in blank material
		Material dummyMaterial;
		Material dummyMaterial2;
		dummyMaterial.power = 1.0f;
		//std::swap(mMasterChiefBody.material, dummyMaterial);
		//std::swap(mMasterChiefHead.material, dummyMaterial2);

		mStandardEffect.Begin();
		//mStandardEffect.Render(mMasterChiefHead);
		//mStandardEffect.Render(mMasterChiefBody);
		mStandardEffect.End();

		mHologramEffect.Begin();
		mHologramEffect.Render(mCortanaHair);
		mHologramEffect.Render(mCortanaEyes);
		mHologramEffect.Render(mCortanaHead);
		mHologramEffect.Render(mCortanaBody);
		mHologramEffect.End();

		// Restore original material
		//std::swap(mMasterChiefBody.material, dummyMaterial);
		//std::swap(mMasterChiefHead.material, dummyMaterial2);
	}
	mBloomRenderTarget.EndRender();

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
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %f %s", mFPS, mPause ? "(Paused!)" : "");
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_OpenOnArrow))
	{
		if (ImGui::DragFloat3("Direction##Light", &mDirectionalLight.direction.x, 0.01f, -1.0f, 1.0f)) {
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}
		
		ImGui::ColorEdit4("Ambient##Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##Light", &mDirectionalLight.specular.r);
	}

	if (ImGui::CollapsingHeader("Ground", ImGuiTreeNodeFlags_OpenOnArrow))
	{
		ImGui::ColorEdit4("Ambient##Ground", &mGroundRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Ground", &mGroundRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Ground", &mGroundRenderObject.material.specular.r);
		ImGui::DragFloat("Power##Ground", &mGroundRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("Master Chief", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		//ImGui::ColorEdit4("Ambient##MasterChief", &mMasterChiefBody.material.ambient.r);
		//mMasterChiefHead.material.ambient = mMasterChiefBody.material.ambient;
		//ImGui::ColorEdit4("Diffuse##MasterChief", &mMasterChiefBody.material.diffuse.r);
		//mMasterChiefHead.material.diffuse = mMasterChiefBody.material.diffuse;
		//ImGui::ColorEdit4("Specular##MasterChief", &mMasterChiefBody.material.specular.r);
		//mMasterChiefHead.material.specular = mMasterChiefBody.material.specular;
		//ImGui::ColorEdit4("Emissive##MasterChief", &mMasterChiefBody.material.emissive.r);
		//mMasterChiefHead.material.emissive = mMasterChiefBody.material.emissive;
		//ImGui::DragFloat("PowerB##MasterChief", &mMasterChiefBody.material.power, 1.0f, 1.0f, 100.0f);
		//mMasterChiefHead.material.power = mMasterChiefBody.material.power;
	}

	// Includes
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mHologramEffect.DebugUI();

	ImGui::Separator();
	mPostProccessingEffect.DebugUI();
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Blur Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragInt("Blur Iteration", &mGaussianBlurEffect.BlurIterations(), 1, 1, 100);
		ImGui::DragFloat("Blur Saturation", &mGaussianBlurEffect.BlurSaturation(), 0.001f, 1.0f, 100.0f);
	}

	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();
	mCrossHatchEffect.DebugUI();
	ImGui::Separator();
	ImGui::End();

	//ImGui::Begin("Render Targets", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	//ImGui::Text("Base");
	//ImGui::Image(mBaseRenderTarget.GetRawData(), { 256, 144 });
	//ImGui::Text("Bloom");
	//ImGui::Image(mBloomRenderTarget.GetRawData(), { 256, 144 });
	//ImGui::Text("Horizontal Blur");
	//ImGui::Image(mGaussianBlurEffect.GetHorizontalBlurTexture().GetRawData(), { 256, 144 });
	//ImGui::Text("Vertical Blur");
	//ImGui::Image(mGaussianBlurEffect.GetVerticalBlurTexture().GetRawData(), { 256, 144 });

	//ImGui::End();
}