#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

namespace
{
	void DrawSprite(TextureId id, const Vector2& pos, float rotation = 0.0f, Pivot pivot = Pivot::Center, Flip flip = Flip::None)
	{
		SpriteRenderer::Get()->Draw(id, pos, rotation, pivot, flip);
	}

	// Check for custom components
	bool OnMake(const char* componentName, const rapidjson::Value& value, GameObject& gameObject)
	{
		return false;
	}
}

Enumerator GameState::MoveCubeLeftTask(float Param)
{
	return [=](CoroPush& yield_return)
	{
		while (mBoxPos.x < 5.0f)
		{
			mBoxPos.x += 0.01f;
			yield_return(nullptr);
		}
	};
}

Enumerator GameState::MoveCubeRightTask(float Param) // WaitForSeconds Test7
{
	return [=](CoroPush& yield_return)
	{
		while (mBoxPos.x < 5.0f)
		{
			mBoxPos.x += 0.01f;
			yield_return(nullptr);
		}
	};
}

void GameState::Initialize()
{
	//GraphicsSystem::Get()->SetClearColor(Colors::Gray);
	mGameWorld.AddService<CameraService>();
	mGameWorld.Initialize(100);

	GameObjectFactory::SetMakeOverride(OnMake);
	auto handle = mGameWorld.CreateGameObject("../../Assets/Templates/fps_camera.json")->GetHandle();

	auto& camera = mGameWorld.GetService<CameraService>()->GetCamera();
	camera.SetPosition({ 0.0f, 10.0f, -20.0f });
	camera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mBoxPos = Vector3(-5.0f, 0.5f, 0.0f);

	mCoroBehaviour.StartCoroutine(MoveCubeRightTask(5.0f));
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);

	if (mCoroBehaviour.HasCoroutines())
	{
		mCoroBehaviour.ResumeCoroutines();
	}
}

void GameState::Render()
{
	mGameWorld.Render();

	SimpleDraw::AddFilledAABB(mBoxPos, {0.5f, 0.5f, 0.5f}, Colors::Purple);

	SimpleDraw::AddPlane(20, Colors::White);
	SimpleDraw::AddTransform(Matrix4::Identity);
	SimpleDraw::Render(mGameWorld.GetService<CameraService>()->GetCamera());
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();
}
