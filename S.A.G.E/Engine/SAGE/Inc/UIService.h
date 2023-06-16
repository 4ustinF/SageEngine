#pragma once

#include "Service.h"

namespace SAGE
{
	class UIService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::UserInterface)

		void Initialize() override;
		void Terminate() override;

		void Update(float deltaTime) override;

		void Render() override;
		void DebugUI() override;

		void DrawSprite(Graphics::TextureId id, const Math::Vector2& pos, float rotation = 0.0f, Input::Pivot pivot = Input::Pivot::Center, Input::Flip flip = Input::Flip::None);
		void DrawSprite(Graphics::TextureId id, const Math::Rect& sourceRect, const Math::Vector2& pos, float rotation = 0.0f, Input::Pivot pivot = Input::Pivot::Center, Input::Flip flip = Input::Flip::None);

		Graphics::UIButton& CreateButton();

	private:
		std::vector<Graphics::UIButton> mButtons;
	};
}