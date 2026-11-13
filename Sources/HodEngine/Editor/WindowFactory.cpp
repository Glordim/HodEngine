#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/WindowFactory.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

namespace hod::inline editor
{
	namespace
	{
		/// @brief Removes the window type from the route and its children, and the children left empty
		/// @return If the route is empty now
		bool RemoveFromRoute(WindowFactory::Route& route, ReflectionDescriptor* reflectionDescriptor)
		{
			for (uint32_t index = 0; index < route._windowReflectionDescriptors.Size(); ++index)
			{
				if (route._windowReflectionDescriptors[index] == reflectionDescriptor)
				{
					route._windowReflectionDescriptors.Erase(index);
					break;
				}
			}

			uint32_t index = 0;
			while (index < route._children.Size())
			{
				if (RemoveFromRoute(*route._children[index], reflectionDescriptor))
				{
					DefaultAllocator::GetInstance().Delete(route._children[index]);
					route._children.Erase(index);
				}
				else
				{
					++index;
				}
			}

			return route._windowReflectionDescriptors.Empty() && route._children.Empty();
		}

		/// @return If the window type is in the route or its children
		bool IsInRoute(const WindowFactory::Route& route, const ReflectionDescriptor* reflectionDescriptor)
		{
			for (const ReflectionDescriptor* windowReflectionDescriptor : route._windowReflectionDescriptors)
			{
				if (windowReflectionDescriptor == reflectionDescriptor)
				{
					return true;
				}
			}
			for (const WindowFactory::Route* child : route._children)
			{
				if (IsInRoute(*child, reflectionDescriptor))
				{
					return true;
				}
			}
			return false;
		}
	}

	_SingletonConstructor(WindowFactory) {}

	const WindowFactory::Route& WindowFactory::GetRootRoute() const
	{
		return _rootRoute;
	}

	bool WindowFactory::Register(const String& routePath, ReflectionDescriptor& reflectionDescriptor)
	{
		// Unregister removes it from every route: one route per window type
		if (IsInRoute(_rootRoute, &reflectionDescriptor))
		{
			OUTPUT_ERROR("WindowFactory: the window '{}' is registered already", reflectionDescriptor.GetDisplayName());
			return false;
		}

		Route* route = &_rootRoute;

		std::string_view routePathView = routePath;
		while (routePathView.empty() == false)
		{
			size_t           separatorIndex = routePathView.find('/');
			std::string_view routeName;
			if (separatorIndex == String::Npos)
			{
				routeName = routePathView;
				routePathView = std::string_view();
			}
			else
			{
				routeName = routePathView.substr(0, separatorIndex);
				routePathView.remove_prefix(separatorIndex + 1);
			}

			if (routeName.empty())
			{
				continue;
			}

			Route* childRoute = nullptr;
			for (Route* it : route->_children)
			{
				if (it->_name == routeName)
				{
					childRoute = it;
					break;
				}
			}
			if (childRoute == nullptr)
			{
				childRoute = DefaultAllocator().GetInstance().New<Route>();
				childRoute->_name = routeName;
				route->_children.push_back(childRoute);
			}
			route = childRoute;
		}

		route->_windowReflectionDescriptors.push_back(&reflectionDescriptor);
		return true;
	}

	void WindowFactory::Unregister(ReflectionDescriptor& reflectionDescriptor)
	{
		RemoveFromRoute(_rootRoute, &reflectionDescriptor);
		ImGuiManager::GetInstance()->DestroyWindows(reflectionDescriptor);
	}

	void WindowFactory::Draw()
	{
		DrawRoute(_rootRoute);
	}

	void WindowFactory::DrawRoute(Route& route)
	{
		for (Route* childRoute : route._children)
		{
			if (ImGui::BeginMenu(childRoute->_name.CStr()))
			{
				DrawRoute(*childRoute);
				ImGui::EndMenu();
			}
		}

		for (ReflectionDescriptor* windowReflectionDescriptor : route._windowReflectionDescriptors)
		{
			if (ImGui::MenuItem(windowReflectionDescriptor->GetDisplayName().CStr()))
			{
				imgui::Window* window = (imgui::Window*)windowReflectionDescriptor->CreateInstance();
				ImGuiManager::GetInstance()->OpenWindow(window);
			}
		}
	}
}
