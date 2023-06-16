#include "Precompiled.h"
#include "UIButton.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;


namespace
{
	void print_hello() {
		int i = 0;
	}
}

UIButton::UIButton()
{
	mButtonActive = true;
	//OnClick = std::bind(ButtonTempFunction);
	//OnClick = std::bind(print_hello);
	//OnClick = print_hello;

	mPosition = { 640.0f, 360.0f };
	mRect.max = Vector2(350.0f, 75.0f);

	SetButtonTexture("ButtonTexture.png");
	SetHoverTexture("ButtonHoverTexture.png");
}

UIButton::~UIButton()
{
	mButtonTextureId = 0;
	mHoverTextureId = 0;
	OnClick = nullptr;
}

void UIButton::SetButtonTexture(const char* id)
{
	mButtonTextureId = TextureManager::Get()->LoadTexture(id);
}

void UIButton::SetHoverTexture(const char* id)
{
	mHoverTextureId = TextureManager::Get()->LoadTexture(id);
}

void UIButton::ButtonTempFunction()
{
	mButtonActive = !mButtonActive;
}

