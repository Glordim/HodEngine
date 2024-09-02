#include "HodEngine/ImGui/Pch.hpp"
#include "HodEngine/ImGui/Window/Window.hpp" 
#include "HodEngine/ImGui/Window/WindowDescription.hpp"
#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_internal.h"

#include <format>

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
		if (ImGui::Begin(GetIdentifier(), &open, _flags) == true)
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
		const char* title = nullptr;
		if (_title.empty())
		{
			title = GetDescription()->GetName();
		}
		else
		{
			title = _title.c_str();
		}
		_identifier = std::format("{}##{}_{}", title, GetDescription()->GetName(), _id, GetDescription()->GetName());
		return _identifier.c_str();
	}

	/// @brief 
	/// @param id 
	void Window::SetId(uint64_t id)
	{
		_id = id;
	}

	/// @brief 
	/// @return 
	uint64_t Window::GetId() const
	{
		return _id;
	}

	/// @brief 
	/// @param flags 
	void Window::SetFlags(ImGuiWindowFlags flags)
	{
		_flags = flags;
	}
	
	/// @brief 
	/// @return 
	ImGuiWindowFlags Window::GetFlags() const
	{
		return _flags;
	}

	/// @brief 
	/// @param title 
	void Window::SetTitle(std::string_view title)
	{
		_title = title;
	}
}
