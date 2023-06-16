#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

GameState::PlanetInfo GameState::InitalizePlanets(float scale = 1.0f, float rotSpeed = 0.0, float dist = 0.0f, float revSpeed = 0.0f)
{
	PlanetInfo info;
	info.scale = scale;
	info.rotationSpeed = rotSpeed;
	info.distFromCen = dist;
	info.revolutionSpeed = revSpeed;

	return info;
}

void GameState::Initialize()
{
	mCamera.SetPosition({0.0f, 30.0f, 6.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});
	mCamera.SetFarPlane(100000);

	// NDC - Normalized Device Coordinate
	//
	//          +-----------------+
	//         /                 /|
	//   1.0  +-----------------+ |
	//	      |        ^        | |
	//	      |        |        | |
	//	      | <------+------> | |
	//	      |        |        | | 1.0
	//	      |        v        |/
	//	-1.0  +-----------------+ 0.0
	//      -1.0               1.0
	//

	//-------------------------------------------------------

	//mCubeTexture.Initialize("../../Assets/Images/space.jpg");
	//mSkyBoxTexture.Initialize("../../Assets/Images/SkyBoxWater.jpg");
	mSkyBoxTexture.Initialize("../../Assets/Images/SkyBoxSpace.png");
	mSunTexture.Initialize("../../Assets/Images/sun.jpg");
	mMercuyTexture.Initialize("../../Assets/Images/Mercury.jpg");
	mVenusTexture.Initialize("../../Assets/Images/Venus.jpg");
	mEarthTexture.Initialize("../../Assets/Images/earth.jpg");
	mMarsTexture.Initialize("../../Assets/Images/Mars.jpg");
	mJupiterTexture.Initialize("../../Assets/Images/Jupiter.jpg");
	mSaturnTexture.Initialize("../../Assets/Images/Saturn.jpg");
	mUranusTexture.Initialize("../../Assets/Images/Uranus.jpg");
	mNeptuneTexture.Initialize("../../Assets/Images/Neptune.jpg");
	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);

	//mCubeMesh = MeshBuilder::CreateCubePX();
	//mCubeBuffer.Initialize(mCubeMesh);

	mSkyBoxMesh = MeshBuilder::CreateSkyBoxPX();
	mSkyBoxBuffer.Initialize(mSkyBoxMesh);

	//mSphereMesh = MeshBuilder::CreateSkydomePX(20, 20, 1.0f);
	//mSphereBuffer.Initialize(mSphereMesh);

	mSphereMesh = MeshBuilder::CreateSpherePX(150, 150, 1.0f);
	mSphereBuffer.Initialize(mSphereMesh);

	mVertexShader.Initialize<VertexPX>(L"../../Assets/Shaders/DoTexturing.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/DoTexturing.fx");

	mConstantBuffer.Initialize(sizeof(Matrix4));

								//scale, rotSpeed, dist, revSpeed
	sunInfo		= InitalizePlanets(2.0f, -1.0f, 0.0f, 0.0f);
	mercuryInfo = InitalizePlanets(0.5f, -2.0f, -4.0f, 2.0f);
	venusInfo	= InitalizePlanets(0.5f,  2.0f, -6.0f, 1.75f);
	earthInfo	= InitalizePlanets(0.5f, -2.0f, -8.0f, 1.5f);
	marsInfo	= InitalizePlanets(0.5f, -2.0f, -10.0f, 1.25f);
	JupiterInfo = InitalizePlanets(1.0f, -2.0f, -12.0f, 1.0f);
	SaturnInfo	= InitalizePlanets(1.0f, -2.0f, -14.0f, 0.75f);
	UranusInfo	= InitalizePlanets(0.75f, 2.0f, -16.0f, 0.5f);
	NeptuneInfo = InitalizePlanets(0.75f, -2.0f, -18.0f, 0.25f);
}

void GameState::Terminate()
{
	mConstantBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mSampler.Terminate();

	//mCubeBuffer.Terminate();
	//mCubeTexture.Terminate();

	mSkyBoxBuffer.Terminate();
	mSkyBoxTexture.Terminate();

	mSphereBuffer.Terminate();

	mSunTexture.Terminate();
	mMercuyTexture.Terminate();
	mVenusTexture.Terminate();
	mEarthTexture.Terminate();
	mMarsTexture.Terminate();
	mJupiterTexture.Terminate();
	mSaturnTexture.Terminate();
	mUranusTexture.Terminate();
	mNeptuneTexture.Terminate();
}

void GameState::UpdatePlanet(PlanetInfo& planet, float deltaTime)
{
	if (planet.revolution > Math::Constants::TwoPi) { planet.revolution = 0.0f; }
	if (planet.rotation > Math::Constants::TwoPi) { planet.rotation = 0.0f; }
	planet.revolution += planet.revolutionSpeed * deltaTime;
	planet.rotation += planet.rotationSpeed * deltaTime;
}

void GameState::Update(float deltaTime)
{
	auto inputSystems = Input::InputSystem::Get();
	float rotation = 0.0f;

	if (inputSystems->IsKeyDown(KeyCode::RIGHT)) {
		rotation += mCameraMoveSpeed;
		world = world * SAGE::Math::Matrix4::RotationY(rotation);
	}
	if (inputSystems->IsKeyDown(KeyCode::LEFT)) {
		rotation -= mCameraMoveSpeed;
		world = world * SAGE::Math::Matrix4::RotationY(rotation);
	}
	if (inputSystems->IsKeyDown(KeyCode::UP)) {
		rotation += mCameraMoveSpeed;
		world = world * SAGE::Math::Matrix4::RotationX(rotation);
	}
	if (inputSystems->IsKeyDown(KeyCode::DOWN)) {
		rotation -= mCameraMoveSpeed;
		world = world * SAGE::Math::Matrix4::RotationX(rotation);
	}

	//Sun
	if (sunInfo.rotation > Math::Constants::TwoPi) { sunInfo.rotation = 0.0f; }
	sunInfo.rotation += sunInfo.rotationSpeed * deltaTime;

	//Planets
	UpdatePlanet(mercuryInfo, deltaTime);
	UpdatePlanet(venusInfo, deltaTime);
	UpdatePlanet(earthInfo, deltaTime);
	UpdatePlanet(marsInfo, deltaTime);
	UpdatePlanet(JupiterInfo, deltaTime);
	UpdatePlanet(SaturnInfo, deltaTime);
	UpdatePlanet(UranusInfo, deltaTime);
	UpdatePlanet(NeptuneInfo, deltaTime);
}

void GameState::RenderPlanet(PlanetInfo& planet, SAGE::Graphics::Texture& texture, Matrix4 view, Matrix4 proj)
{
	Matrix4 Scale = Matrix4::Scaling({ planet.scale, planet.scale, planet.scale });
	Matrix4 Rot = Matrix4::RotationY(planet.rotation);
	Matrix4 Tran = Matrix4::Translation({ planet.distFromCen, 0.0f, 0.0f });
	Matrix4 Rev = Matrix4::Translation({ cos(planet.revolution) * planet.distFromCen - planet.distFromCen, 0.0f, sin(planet.revolution) * planet.distFromCen });
	Matrix4 WVP = Transpose(Scale * Rot * Tran * Rev * world * view * proj);
	mConstantBuffer.Update(&WVP);
	texture.BindPS(0);
	mSphereBuffer.Render();
}

void GameState::Render()
{
	mVertexShader.Bind();
	mPixelShader.Bind();
	mSampler.BindPS(0);
	mConstantBuffer.BindVS(0);

	Matrix4 view = mCamera.GetViewMatrix();
	Matrix4 proj = mCamera.GetProjectionMatrix();

	//Cube
	//Matrix4 cubeWVP = Transpose(world * view * proj);
	//mConstantBuffer.Update(&cubeWVP);
	//mCubeTexture.BindPS(0);
	//mCubeBuffer.Render();

	//SkyBox
	const float skyboxSize = 50000.0f;
	Matrix4 skyBoxWorld = Matrix4::Identity * Matrix4::Scaling({ skyboxSize, skyboxSize, skyboxSize });
	Matrix4 SkyBoxWVP = Transpose(world * skyBoxWorld * view * proj);
	mConstantBuffer.Update(&SkyBoxWVP);
	mSkyBoxTexture.BindPS(0);
	mSkyBoxBuffer.Render();

	////SkyDome
	//const float skyDomeSize = 40000.0f;
	//Matrix4 skyDomeWorld = Matrix4::Identity * Matrix4::Scaling({ skyDomeSize, skyDomeSize, skyDomeSize });
	//Matrix4 SkyDomeWVP = Transpose(world * skyDomeWorld * view * proj);
	//mConstantBuffer.Update(&SkyDomeWVP);
	//mEarthTexture.BindPS(0);
	//mSphereBuffer.Render();

	//world = [Rotation][Translate][Revolution] - Moon[Translate][Revolution]
	//Sun
	Matrix4 sunScale = Matrix4::Scaling({ sunInfo.scale, sunInfo.scale, sunInfo.scale });
	Matrix4 sunRot = Matrix4::RotationY(sunInfo.rotation);
	Matrix4 sunWVP = Transpose(sunScale * sunRot * world * view * proj);
	mConstantBuffer.Update(&sunWVP);
	mSunTexture.BindPS(0);
	mSphereBuffer.Render();

	//Planets
	RenderPlanet(mercuryInfo, mMercuyTexture, view, proj);
	RenderPlanet(venusInfo, mVenusTexture, view, proj);
	RenderPlanet(earthInfo, mEarthTexture, view, proj);
	RenderPlanet(marsInfo, mMarsTexture, view, proj);
	RenderPlanet(JupiterInfo, mJupiterTexture, view, proj);
	RenderPlanet(SaturnInfo, mSaturnTexture, view, proj);
	RenderPlanet(UranusInfo, mUranusTexture, view, proj);
	RenderPlanet(NeptuneInfo, mNeptuneTexture, view, proj);
}

void GameState::DebugUI()
{
}