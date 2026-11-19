#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/GameEditorRegistrar.hpp"
#include "HodEngine/GameEditor/ComponentGizmo/ComponentGizmoRegistry.hpp"
#include "HodEngine/GameEditor/ComponentInspector/ComponentInspectorRegistry.hpp"
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
	/// @param descriptor
	/// @param gizmo
	/// @return
	bool GameEditorRegistrar::RegisterComponentGizmo(const ReflectionDescriptor& descriptor, ComponentGizmoBase* gizmo)
	{
		if (ComponentGizmoRegistry::Register(descriptor, gizmo) == false)
		{
			return false;
		}
		_registration.AddUnregister([&descriptor]() { ComponentGizmoRegistry::Unregister(descriptor); });
		return true;
	}

	/// @brief
	/// @param descriptor
	/// @param inspector
	/// @return
	bool GameEditorRegistrar::RegisterComponentInspector(const ReflectionDescriptor& descriptor, ComponentInspectorBase* inspector)
	{
		if (ComponentInspectorRegistry::Register(descriptor, inspector) == false)
		{
			return false;
		}
		_registration.AddUnregister([&descriptor]() { ComponentInspectorRegistry::Unregister(descriptor); });
		return true;
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
