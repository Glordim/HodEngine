#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/GameEditorRegistrar.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportDebugView.hpp"

namespace hod::inline editor
{
	/// @brief
	/// @param registration
	GameEditorRegistrar::GameEditorRegistrar(ModuleRegistration& registration)
	: _registration(registration)
	{
	}

	/// @brief
	/// @param type
	/// @return
	bool GameEditorRegistrar::RegisterViewportDebugView(const ViewportDebugViewType& type)
	{
		if (ViewportDebugViewRegistry::Register(type) == false)
		{
			return false;
		}
		_registration.AddUnregister([&type]() { ViewportDebugViewRegistry::Unregister(type); });
		return true;
	}

	/// @brief
	/// @param listener
	/// @return
	bool GameEditorRegistrar::RegisterViewportGamePointer(ViewportGamePointer::Listener listener)
	{
		if (ViewportGamePointer::Register(listener) == false)
		{
			return false;
		}
		_registration.AddUnregister([listener]() { ViewportGamePointer::Unregister(listener); });
		return true;
	}

	/// @brief
	/// @param draw
	/// @param setupPlayCamera
	void GameEditorRegistrar::SetViewportWorldRenderer(ViewportWorldRenderer::DrawFunction draw, ViewportWorldRenderer::SetupPlayCameraFunction setupPlayCamera)
	{
		ViewportWorldRenderer::Set(draw, setupPlayCamera);
		_registration.AddUnregister([]() { ViewportWorldRenderer::Set(nullptr, nullptr); });
	}
}
