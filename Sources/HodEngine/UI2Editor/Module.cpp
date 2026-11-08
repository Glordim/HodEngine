#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/UI2Editor/EasingPreview.hpp"
#include "HodEngine/UI2Editor/CustomPropertyDrawer/NodeRefCustomPropertyDrawer.hpp"
#include "HodEngine/UI2Editor/CustomPropertyDrawer/RectOffsetsCustomPropertyDrawer.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabAssetType.hpp"
#include "HodEngine/UI2Editor/CustomComponentDrawer/UIComponentCustomComponentDrawer.hpp"

#include <HodEngine/GameEditor/EntityBasedTabEditor/ViewportGamePointer.hpp>


#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/UI2/AnimationClip.hpp>
#include <HodEngine/UI2/CanvasManager.hpp>
#include <HodEngine/UI2/Components/UIComponent.hpp>
#include <HodEngine/UI2/Margin.hpp>
#include <HodEngine/UI2/NodeRef.hpp>
#include <HodEngine/UI2/Padding.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

namespace
{
	/// @brief The easing enum as UI2 registered it on its properties (a Button's TransitionEasing...): each module (DLL) has its
	/// own instance of an enum's descriptor, this one's is UI2's.
	const EnumDescriptor* GetEasingEnumDescriptor()
	{
		ReflectionPropertyVariable* property = ui2::AnimationKey::GetReflectionDescriptor().FindProperty<ReflectionPropertyVariable>("Easing");
		return (property != nullptr) ? property->GetEnumDescriptor() : nullptr;
	}

	/// @brief Playing, the game is drawn in the viewports: its canvases get the pointer from there, not from the main window
	void GiveGamePointerToCanvases(const Vector2& position, const Vector2& resolution, bool pressed, const Vector2& scroll)
	{
		if (ui2::CanvasManager* canvasManager = ui2::CanvasManager::GetInstance())
		{
			canvasManager->SetPointer(position, resolution, pressed, scroll);
		}
	}
}

HOD_STARTUP_EDITOR_MODULE(UI2Editor, registration)
{
	// The same cross for both, pointing the way each one pushes
	registration.RegisterCustomPropertyDrawer(ui2::Margin::GetReflectionDescriptor(),
	                                          DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Outward));
	registration.RegisterCustomPropertyDrawer(ui2::Padding::GetReflectionDescriptor(),
	                                          DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Inward));

	registration.RegisterCustomPropertyDrawer<ui2::NodeRefBase, NodeRefCustomPropertyDrawer>();

	if (const EnumDescriptor* easingEnum = GetEasingEnumDescriptor())
	{
		registration.RegisterEnumValuePreview(*easingEnum, +[](uint64_t value) { DrawEasingPreview((ui2::AnimationKey::Easing)value); });
	}

	registration.RegisterAssetType<UIPrefabAssetType>();
	registration.RegisterCustomComponentDrawer<ui2::UIComponent, UIComponentCustomComponentDrawer>();

	if (ui2::CanvasManager* canvasManager = ui2::CanvasManager::GetInstance())
	{
		canvasManager->SetPointerSource(ui2::CanvasManager::PointerSource::Manual);
	}
	ViewportGamePointer::Register(&GiveGamePointerToCanvases);

	return 0;
}

HOD_SHUTDOWN_EDITOR_MODULE(UI2Editor)
{
	ViewportGamePointer::Unregister(&GiveGamePointerToCanvases);
	if (ui2::CanvasManager* canvasManager = ui2::CanvasManager::GetInstance())
	{
		canvasManager->SetPointerSource(ui2::CanvasManager::PointerSource::Window);
	}
	return 0;
}
