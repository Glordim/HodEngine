#pragma once
#include "HodEngine/Editor/Export.hpp"
#include <HodEngine/Core/Singleton.hpp>

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline editor
{
	class HOD_EDITOR_API WindowFactory
	{
		_Singleton(WindowFactory)

	public:

		struct Route
		{
			String 							_name;
			Vector<ReflectionDescriptor*>	_windowReflectionDescriptors;
			Vector<Route*>					_children;
		};

	public:

		/// @brief In the "Window" menu, under route
		/// @return false (with an error) if the window type is registered already (under any route): the first one stays
		template<typename _Window_>
		bool	Register(const String& route);
		bool	Register(const String& route, ReflectionDescriptor& reflectionDescriptor);
		/// @brief By the editor modules when they unload: from all the routes, destroying the open windows of this type
		void	Unregister(ReflectionDescriptor& reflectionDescriptor);

		const Route&	GetRootRoute() const;

		void	Draw();
		void	DrawRoute(Route& route);

	private:

		Route	_rootRoute;
	};

	template<typename _Window_>
	bool WindowFactory::Register(const String& route)
	{
		return Register(route, _Window_::GetReflectionDescriptor());
	}
}
