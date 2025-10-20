#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/WindowFactory.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

namespace hod::editor
{
	_SingletonConstructor(WindowFactory) {}

	const WindowFactory::Route& WindowFactory::GetRootRoute() const
	{
		return _rootRoute;
	}

	void WindowFactory::Register(const String& routePath, ReflectionDescriptor& reflectionDescriptor)
	{
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
				imgui::ImGuiManager::GetInstance()->OpenWindow(window);
			}
		}
	}
}
