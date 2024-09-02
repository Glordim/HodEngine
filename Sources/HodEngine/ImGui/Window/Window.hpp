#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include "HodEngine/ImGui/Window/WindowDescription.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include "HodEngine/Core/Type.hpp"

#include <string>
#include <cstring>
#include <functional>

namespace hod::imgui
{
	/// @brief 
	class HOD_IMGUI_API Window
	{
		BASE_META_TYPE(Window);

	public:

									Window();
		virtual						~Window() = default;

	public:

		virtual bool				Draw();
		
		virtual void				DrawContent() = 0;

		virtual WindowDescription*	GetDescription() = 0;

		bool						IsClosed() const;
		void						Close();

		const char*					GetIdentifier();

		void						SetId(uint64_t id);
		uint64_t					GetId() const;

		void						SetFlags(ImGuiWindowFlags flags);
		ImGuiWindowFlags			GetFlags() const;

		void						SetTitle(std::string_view title);

	private:

		bool						_closed = false;

		std::string					_title;
		std::string					_identifier;
		uint64_t					_id = 0;

		ImGuiWindowFlags			_flags = 0;
	};
}
