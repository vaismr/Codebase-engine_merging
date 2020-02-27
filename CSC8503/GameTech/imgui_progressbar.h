#pragma once
#include <imgui/imgui.h>

namespace ImGui {
	bool ProgressBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
}
