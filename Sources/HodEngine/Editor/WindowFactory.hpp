#pragma once
#include "HodEngine/Editor/Export.hpp"
#include <HodEngine/Core/Singleton.hpp>

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::editor
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

		template<typename _Window_>
		void	Register(const String& route);
		void	Register(const String& route, ReflectionDescriptor& reflectionDescriptor);

		const Route&	GetRootRoute() const;

		void	Draw();
		void	DrawRoute(Route& route);

	private:

		Route	_rootRoute;
	};

	template<typename _Window_>
	void WindowFactory::Register(const String& route)
	{
		Register(route, _Window_::GetReflectionDescriptor());
	}
}
