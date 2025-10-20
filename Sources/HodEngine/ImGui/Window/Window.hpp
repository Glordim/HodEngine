#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include "HodEngine/Core/Type.hpp"

#include "HodEngine/Core/String.hpp"
#include <cstring>
#include <functional>

namespace hod::imgui
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

	private:
		bool _closed = false;

		String   _title;
		String   _identifier;
		uint64_t _id = 0;

		ImGuiWindowFlags _flags = 0;
	};
}
