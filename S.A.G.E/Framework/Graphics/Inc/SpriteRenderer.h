#pragma once

#include "Input/Inc/InputTypes.h"

namespace DirectX { class CommonStates; class SpriteBatch; }

namespace SAGE::Graphics
{
	class Texture;

	class SpriteRenderer
	{
	public:
		static void StaticInitialize();
		static void StaticTerminate();
		static SpriteRenderer* Get();

	public:
		SpriteRenderer();
		~SpriteRenderer();

		SpriteRenderer(const SpriteRenderer&) = delete;
		SpriteRenderer& operator=(const SpriteRenderer&) = delete;

		void Initialize();
		void Terminate();

		void SetTransform(const Math::Matrix4& transform);

		void BeginRender();
		void EndRender();

		void Draw(const std::size_t& textureID, const Math::Vector2& pos, float rotation = 0.0f, Input::Pivot pivot = Input::Pivot::Center, Input::Flip flip = Input::Flip::None);
		void Draw(const std::size_t& textureID, const Math::Rect& sourceRect, const Math::Vector2& pos, float rotation = 0.0f, Input::Pivot pivot = Input::Pivot::Center, Input::Flip flip = Input::Flip::None);

		void Draw(const Texture& texture, const Math::Vector2& pos, float rotation = 0.0f, Input::Pivot pivot = Input::Pivot::Center, Input::Flip flip = Input::Flip::None);
		void Draw(const Texture& texture, const Math::Rect& sourceRect, const Math::Vector2& pos, float rotation = 0.0f, Input::Pivot pivot = Input::Pivot::Center, Input::Flip flip = Input::Flip::None);

	private:
		friend class Font;

		DirectX::CommonStates* mCommonStates;
		DirectX::SpriteBatch* mSpriteBatch;

		Math::Matrix4 mTransform;
	};
}