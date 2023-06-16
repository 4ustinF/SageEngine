#include <SAGE/Inc/SAGE.h>

using namespace SAGE;

class HotPinkState : public AppState
{
	void Initialize() override
	{
		auto graphicsSystem = Graphics::GraphicsSystem::Get();
		graphicsSystem->SetClearColor(Graphics::Colors::HotPink);
	}

	void Update(float deltaTime) override
	{
		if (GetAsyncKeyState('2')) {
			MainApp().ChangeState("DarkBlue");
		}
	}
};

class DarkBlueState : public AppState
{
	void Initialize() override
	{
		auto graphicsSystem = Graphics::GraphicsSystem::Get();
		graphicsSystem->SetClearColor(Graphics::Colors::DarkBlue);
	}

	void Update(float deltaTime) override
	{
		if (GetAsyncKeyState('1')) {
			MainApp().ChangeState("HotPink");
		}
	}
};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	auto& app = MainApp();
	app.AddState<HotPinkState>("HotPink");
	app.AddState<DarkBlueState>("DarkBlue");
	app.Run({L"Hello DirectX", 1280, 720});

	return 0;
}