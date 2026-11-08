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

	namespace
	{
		/// @brief The center of the window drawing now: from a menu or a popup, the window they belong to; the main menu bar (or no
		/// window): its OS window's work area
		ImVec2 GetCurrentWindowCenter()
		{
			ImGuiWindow* window = ImGui::GetCurrentContext() != nullptr ? ImGui::GetCurrentContext()->CurrentWindow : nullptr;
			while (window != nullptr && window->ParentWindow != nullptr &&
			       (window->Flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_ChildMenu | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Tooltip)) != 0)
			{
				window = window->ParentWindow;
			}
			if (window == nullptr || window->Viewport == nullptr)
			{
				return ImGui::GetMainViewport()->GetWorkCenter();
			}
			if (std::strcmp(window->Name, "##MainMenuBar") == 0) // A bar along the OS window (see ImGui::BeginMainMenuBar)
			{
				return window->Viewport->GetWorkCenter();
			}
			return ImVec2(window->Pos.x + window->Size.x * 0.5f, window->Pos.y + window->Size.y * 0.5f);
		}
	}

	/// @brief
	void Window::CenterOnCurrentWindow()
	{
		_centerOnNextDraw = true;
		_centerPosition = GetCurrentWindowCenter();
	}

	/// @brief
	/// @param size
	void Window::SetInitialSize(const ImVec2& size)
	{
		_initialSize = size;
	}

	/// @brief
	void Window::CenterNextWindowOnCurrentWindow()
	{
		ImGui::SetNextWindowPos(GetCurrentWindowCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
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
		if (_initialSize.x > 0.0f && _initialSize.y > 0.0f)
		{
			ImGui::SetNextWindowSize(_initialSize, ImGuiCond_FirstUseEver);
		}
		if (_centerOnNextDraw)
		{
			// Always, not Appearing: its position saved from a previous session would win otherwise
			ImGui::SetNextWindowPos(_centerPosition, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			_centerOnNextDraw = false;
		}
		PushDockTabStyle();
		bool visible = ImGui::Begin(GetIdentifier(), &open, _flags);
		PopDockTabStyle();
		if (visible)
		{
			DrawDockTabContextMenu();
			ExtendClipRectToWindowLeftEdge();
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
	void Window::PushDockTabStyle()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, DockTabFramePadding);
	}

	/// @brief
	void Window::PopDockTabStyle()
	{
		ImGui::PopStyleVar();
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

	/// @brief The inner clip rect starts after the left border: let the content draw up to the window edge (override markers of the inspectors),
	/// never outside the window. Must be called right after ImGui::Begin()
	void Window::ExtendClipRectToWindowLeftEdge()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImRect       cliprect = window->ClipRect;
		cliprect.Min.x = window->Pos.x;
		ImGui::PopClipRect();
		ImGui::PushClipRect(cliprect.Min, cliprect.Max, false);
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
