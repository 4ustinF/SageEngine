#pragma once

#include "TextureManager.h"

namespace SAGE::Graphics
{
	class UIButton
	{
	public:
		UIButton();
		~UIButton();

		void SetButtonActive(bool isActive) { mButtonActive = isActive; }
		//void SetOnClick(const std::function<void()>& func) { OnClick = func; } // Method for setting a function to be called when the button is clicked
		void SetButtonPosition(SAGE::Math::Vector2 position) { mPosition = position; };
		void SetRect(const SAGE::Math::Rect& rect) { mRect = rect; };
		void SetButtonTexture(const char* id);
		void SetHoverTexture(const char* id);

		bool GetButtonActive() { return mButtonActive; }
		SAGE::Math::Vector2 GetPosition() { return mPosition; }
		SAGE::Math::Rect GetRect() { return mRect; }
		TextureId GetButtonTextureId() { return mButtonTextureId; }
		TextureId GetHoverTextureId() { return mHoverTextureId; }

		//std::function<void()> OnClick;  // Function to be called when the button is clicked
		void (*OnClick)();  // Function to be called when the button is clicked

	private:
		void ButtonTempFunction();

		bool mButtonActive = true;

		SAGE::Math::Vector2 mPosition;
		SAGE::Math::Rect mRect;
		 
		TextureId mButtonTextureId = 0;
		TextureId mHoverTextureId = 0;
	};
}

/*
button.onClick([]() {
  std::cout << "Button was clicked!" << std::endl;
});
*/
