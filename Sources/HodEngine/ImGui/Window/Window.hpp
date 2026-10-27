#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include "HodEngine/Core/TypeTrait.hpp"

#include "HodEngine/Core/String.hpp"
#include <cstring>
#include <functional>

namespace hod::inline imgui
{
	/// @brief
	class HOD_IMGUI_API Window
	{
		REFLECTED_CLASS_NO_PARENT(Window)

	public:
		Window();
		virtual ~Window() = default;

	public:
		virtual bool Draw();

		virtual void DrawContent() = 0;

		bool IsClosed() const;
		void Close();

		const char* GetIdentifier();

		void     SetId(uint64_t id);
		uint64_t GetId() const;

		void             SetFlags(ImGuiWindowFlags flags);
		ImGuiWindowFlags GetFlags() const;

		void SetTitle(std::string_view title);

	public:
		// ImGui has no dedicated tab padding: dock tab bars and docked window title bars use style.FramePadding.
		// Push it around ImGui::Begin() of dockable windows, ImGui::DockSpace() and ImGui::NewFrame() (floating dock nodes).
		static constexpr ImVec2 DockTabFramePadding = ImVec2(10.0f, 6.0f);

		static void PushDockTabStyle();
		static void PopDockTabStyle();

	protected:
		void DrawDockTabContextMenu();

		static void ExtendClipRectToWindowLeftEdge();

	private:
		bool _closed = false;

		String   _title;
		String   _identifier;
		uint64_t _id = 0;

		ImGuiWindowFlags _flags = 0;
	};
}
