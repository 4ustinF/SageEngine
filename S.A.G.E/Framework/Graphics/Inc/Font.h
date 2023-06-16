#pragma once

#include "Colors.h"

interface IFW1Factory;
interface IFW1FontWrapper;

namespace SAGE::Graphics
{
	class Font
	{
	public:
		static void StaticInitialize();
		static void StaticTerminate();
		static Font* Get();

	public:
		Font();
		~Font();

		void Initialize();
		void Terminate();

		float GetStringWidth(const wchar_t* str, float size) const;
		void Draw(const char* str, float xPos, float yPos, float size, Color color);
		void Draw(const char* str, Math::Vector2 position, float size, Color color);

	private:
		IFW1Factory* mFW1Factory;
		IFW1FontWrapper* mFontWrapper;
	};

}