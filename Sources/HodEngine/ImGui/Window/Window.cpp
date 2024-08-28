#include "HodEngine/ImGui/Pch.hpp"
#include "HodEngine/ImGui/Window/Window.hpp" 
#include "HodEngine/ImGui/Window/WindowDescription.hpp"
#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_internal.h"

namespace hod::imgui
{
	/// @brief 
	Window::Window()
	{
		
	}

	/// @brief 
	/// @return 
	bool Window::IsClosed() const
	{
		return _closed;
	}

	/// @brief 
	void Window::Close()
	{
		_closed = true;
	}

	/// @brief 
	/// @return 
	bool Window::Draw()
	{
		bool open = true;
		if (ImGui::Begin(GetIdentifier(), &open) == true)
		{
			ImRect cliprect = ImGui::GetCurrentWindow()->ClipRect;
			cliprect.Min.x -= ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Min.y -= ImGui::GetStyle().WindowPadding.y * 0.5f;
			cliprect.Max.x += ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Max.y += ImGui::GetStyle().WindowPadding.y * 0.5f;
			ImGui::PopClipRect();
			ImGui::PushClipRect(cliprect.Min, cliprect.Max, false);
			DrawContent();
		}
		if (open == false)
		{
			Close();
		}
		ImGui::End();
		return open;
	}

	/// @brief 
	/// @return 
	const char* Window::GetIdentifier()
	{
		_identifier = GetDescription()->GetName();
		return _identifier.c_str();
	}
}
