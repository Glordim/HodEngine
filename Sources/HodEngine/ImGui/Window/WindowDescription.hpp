#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include "HodEngine/Core/Type.hpp"

#include <string>
#include <cstring>
#include <functional>

namespace hod::imgui
{
	class Window;

	class HOD_IMGUI_API WindowDescription
	{
	public:

		WindowDescription(const char* name, bool unique, std::function<Window*()> allocateFunction);

		Window* InstantiateWindow();

		const char* GetName() const;

		bool IsUnique() const;

		static WindowDescription* GetDescription(const char* name);

	private:

		const char* _name;
		bool		_unique;
		std::function<Window*()> _allocateFunction;

		static std::vector<WindowDescription*>	_descriptions;
	};
}

#define WINDOW_DESCRIPTION()											\
	private:															\
		static hod::imgui::WindowDescription _description;				\
	public:																\
		static hod::imgui::WindowDescription* GetStaticDescription()	\
		{																\
			return &_description;										\
		}																\
		hod::imgui::WindowDescription* GetDescription() override		\
		{																\
			return GetStaticDescription();								\
		}																\

#define DECLARE_WINDOW_DESCRIPTION(Window, Name, Unique)	\
	hod::imgui::WindowDescription Window::_description(		\
		Name,												\
		Unique,												\
		[](){ return new Window; }							\
	);														\
