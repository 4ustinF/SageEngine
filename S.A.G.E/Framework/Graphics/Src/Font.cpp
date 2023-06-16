#include "Precompiled.h"
#include "Font.h"

#include "GraphicsSystem.h"
#include <FW1FontWrapper/Inc/FW1FontWrapper.h>

using namespace SAGE;
using namespace SAGE::Graphics;

namespace
{
	const wchar_t* sFontFamily = L"Consolas";
	Font* mFont = nullptr;

	inline uint32_t ToColor(const Color& color)
	{
		uint8_t r = (uint8_t)(color.r * 255);
		uint8_t g = (uint8_t)(color.g * 255);
		uint8_t b = (uint8_t)(color.b * 255);
		return 0xff000000 | (b << 16) | (g << 8) | r;
	}
}

void Font::StaticInitialize()
{
	ASSERT(mFont == nullptr, "[Font] System already initialized!");
	mFont = new Font();
	mFont->Initialize();
}

void Font::StaticTerminate()
{
	if (mFont != nullptr)
	{
		mFont->Terminate();
		SafeDelete(mFont);
	}
}

Font* Font::Get()
{
	ASSERT(mFont != nullptr, "[Font] No instance registered.");
	return mFont;
}

Font::Font()
	: mFW1Factory(nullptr)
	, mFontWrapper(nullptr)
{
}

Font::~Font()
{
}

void Font::Initialize()
{
	ID3D11Device* device = GraphicsSystem::Get()->GetDevice();
	FW1CreateFactory(FW1_VERSION, &mFW1Factory);
	auto hr = mFW1Factory->CreateFontWrapper(device, sFontFamily, &mFontWrapper);
	ASSERT(SUCCEEDED(hr), "[Font] Failed to initialize FW1FontWrapper. Error = %x", hr);
}

void Font::Terminate()
{
	SafeRelease(mFontWrapper);
	SafeRelease(mFW1Factory);
}

float Font::GetStringWidth(const wchar_t* str, float size) const
{
	FW1_RECTF layoutRect;
	layoutRect.Left = 0.0f;
	layoutRect.Top = 0.0f;
	layoutRect.Right = (float)GraphicsSystem::Get()->GetBackBufferWidth();
	layoutRect.Bottom = (float)GraphicsSystem::Get()->GetBackBufferHeight();
	auto rect = mFontWrapper->MeasureString(str, sFontFamily, size, &layoutRect, FW1_RESTORESTATE);
	return rect.Right - rect.Left;
}

void Font::Draw(const char* str, float x, float y, float size, Color color)
{
	int len = (int)strlen(str);
	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
	std::wstring wstr(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, str, len, &wstr[0], sizeNeeded);

	ID3D11DeviceContext* context = GraphicsSystem::Get()->GetContext();
	mFontWrapper->DrawString(context, wstr.c_str(), size, x, y, ToColor(color), FW1_RESTORESTATE);
}

void Font::Draw(const char* str, Math::Vector2 position, float size, Color color)
{
	Draw(str, position.x, position.y, size, color);
}