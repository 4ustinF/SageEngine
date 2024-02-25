#pragma once

#include "imgui.h"

struct AppLog
{
	ImGuiTextBuffer textbuffer;
	bool scrollToBottom;

	void clear()
	{
		textbuffer.clear();
	}

	void AddLog(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		textbuffer.appendfv(fmt, args);
		va_end(args);
		scrollToBottom = true;
	}

	void Draw(const char* title, bool* p_opened = nullptr)
	{
		ImGui::Begin(title, p_opened);
		ImGui::TextUnformatted(textbuffer.begin());
		if (scrollToBottom)
		{
			ImGui::SetScrollHereX(1.0f);
			ImGui::SetScrollHereY(1.0f);
			scrollToBottom = false;
		}
		ImGui::End();
	}
};