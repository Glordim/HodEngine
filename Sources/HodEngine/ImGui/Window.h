#pragma once

#include "HodEngine/Core/Type.h"

#include <string>
#include <functional>

namespace hod::imgui
{
	class Window;

	class WindowDescription
	{
	public:

		WindowDescription(const char* name, bool unique, std::function<Window*()> allocateFunction)
		: _name(name)
		, _unique(unique)
		, _allocateFunction(allocateFunction)
		{
			_descriptions.push_back(this);
		}

		Window* InstantiateWindow()
		{
			return _allocateFunction();
		}

		const char* GetName() const
		{
			return _name;
		}

		bool IsUnique() const
		{
			return _unique;
		}

		static WindowDescription* GetDescription(const char* name)
		{
			for (WindowDescription* description : _descriptions)
			{
				if (std::strcmp(description->GetName(), name) == 0)
				{
					return description;
				}
			}

			return nullptr;
		}

	private:

		const char* _name;
		bool		_unique;
		std::function<Window*()> _allocateFunction;

		static std::vector<WindowDescription*>	_descriptions;
	};

	#define WINDOW_DESCRIPTION()										\
	private:															\
		static hod::imgui::WindowDescription _description;				\
		static hod::imgui::WindowDescription* GetStaticDescription()	\
		{																\
			return &_description;										\
		}																\
	public:																\
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

	/// @brief 
	class Window
	{
		BASE_META_TYPE(Window);

	public:

							Window();
		virtual				~Window() = default;

	public:

		virtual void			Draw() = 0;
		virtual WindowDescription*	GetDescription() = 0;

		bool				IsClosed() const;
		void				Close();

		const char*			GetIdentifier();

	private:

		bool				_closed = false;

		std::string			_identifier;
	};
}
