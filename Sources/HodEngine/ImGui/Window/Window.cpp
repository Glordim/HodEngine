#include "HodEngine/ImGui/Pch.hpp"
#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_internal.h"
#include "HodEngine/ImGui/Window/Window.hpp"

#include <format>

namespace hod::inline imgui
{
	DESCRIBE_REFLECTED_CLASS(Window, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	Window::Window() {}

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
			DrawDockTabContextMenu();
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

	/// @brief Right-click menu on the dock tab, replaces the dock node window menu button (style.WindowMenuButtonPosition = ImGuiDir_None).
	/// Must be called right after ImGui::Begin(), while the last item is still the dock tab.
	void Window::DrawDockTabContextMenu()
	{
		// GetCurrentWindowRead(): GetCurrentWindow() sets WriteAccessed, which makes IsItemHovered() reject the tab item (MoveId)
		ImGuiWindow* window = ImGui::GetCurrentWindowRead();
		if (window->DockIsActive == false || window->DockNode->Windows.Size != 1)
		{
			return;
		}

		if (ImGui::BeginPopupContextItem("##DockTabContextMenu"))
		{
			ImGuiDockNode* node = window->DockNode;
			if (ImGui::MenuItem("Hide tab bar", nullptr, node->IsHiddenTabBar()))
			{
				node->WantHiddenTabBarToggle = true;
			}
			ImGui::EndPopup();
		}
	}

	/// @brief
	/// @return
	const char* Window::GetIdentifier()
	{
		const char* title = nullptr;
		if (_title.Empty())
		{
			title = GetReflectionDescriptorV().GetDisplayName().CStr();
		}
		else
		{
			title = _title.CStr();
		}
		_identifier = fmt::format("{}###{}_{}", title, GetReflectionDescriptorV().GetTypeName(), _id);
		return _identifier.CStr();
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
