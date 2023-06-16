#include "Precompiled.h"
#include "UIService.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void UIService::Initialize()
{
	SetServiceName("UI Service");
}

void UIService::Terminate()
{
	mButtons.clear();
}

void UIService::Update(float deltaTime)
{

}

void UIService::Render()
{
	auto inputSystem = InputSystem::Get();

	for (auto& b : mButtons)
	{
		if (b.GetButtonActive())
		{
			DrawSprite(b.GetButtonTextureId(), b.GetRect(), b.GetPosition(), 0.0f, Pivot::Center, Flip::None);

			const Vector2 pos = b.GetPosition();
			const Vector2 offSet = b.GetRect().max * 0.5f;
			const Vector2 min = pos - offSet;
			const Vector2 max = pos + offSet;
			const Vector2Int mouse = { inputSystem->GetMouseScreenX(), inputSystem->GetMouseScreenY() };

			// Mouse Hover
			if (mouse.x >= min.x && mouse.x <= max.x && mouse.y >= min.y && mouse.y <= max.y)
			{
				DrawSprite(b.GetHoverTextureId(), b.GetRect(), b.GetPosition(), 0.0f, Pivot::Center, Flip::None);

				// Mouse Click
				if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
				{
					b.OnClick();
				}
			}
		}
	}
}

void UIService::DebugUI()
{

}

void UIService::DrawSprite(TextureId id, const Vector2& pos, float rotation, Pivot pivot, Flip flip)
{
	SpriteRenderer::Get()->Draw(id, pos, rotation, pivot, flip);
}

void UIService::DrawSprite(TextureId id, const Rect& sourceRect, const Vector2& pos, float rotation, Pivot pivot, Flip flip)
{
	SpriteRenderer::Get()->Draw(id, sourceRect, pos, rotation, pivot, flip);
}

UIButton& UIService::CreateButton()
{
	UIButton button;
	return mButtons.emplace_back(button);
}

