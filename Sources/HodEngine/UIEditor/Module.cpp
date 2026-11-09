#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/UIEditor/EasingPreview.hpp"
#include "HodEngine/UIEditor/CustomPropertyDrawer/NodeRefCustomPropertyDrawer.hpp"
#include "HodEngine/UIEditor/CustomPropertyDrawer/RectOffsetsCustomPropertyDrawer.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabAssetType.hpp"
#include "HodEngine/UIEditor/CustomComponentDrawer/UIComponentCustomComponentDrawer.hpp"

#include <HodEngine/GameEditor/EntityBasedTabEditor/ViewportGamePointer.hpp>


#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/UI/AnimationClip.hpp>
#include <HodEngine/UI/CanvasManager.hpp>
#include <HodEngine/UIComponents/Components/UIComponent.hpp>
#include <HodEngine/UI/Margin.hpp>
#include <HodEngine/UI/NodeRef.hpp>
#include <HodEngine/UI/Padding.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

namespace
{
	/// @brief The easing enum as UI registered it on its properties (a Button's TransitionEasing...): each module (DLL) has its
	/// own instance of an enum's descriptor, this one's is UI's.
	const EnumDescriptor* GetEasingEnumDescriptor()
	{
		ReflectionPropertyVariable* property = ui::AnimationKey::GetReflectionDescriptor().FindProperty<ReflectionPropertyVariable>("Easing");
		return (property != nullptr) ? property->GetEnumDescriptor() : nullptr;
	}

	/// @brief Playing, the game is drawn in the viewports: its canvases get the pointer from there, not from the main window
	void GiveGamePointerToCanvases(const Vector2& position, const Vector2& resolution, bool pressed, const Vector2& scroll)
	{
		if (ui::CanvasManager* canvasManager = ui::CanvasManager::GetInstance())
		{
			canvasManager->SetPointer(position, resolution, pressed, scroll);
		}
	}
}

HOD_STARTUP_EDITOR_MODULE(UIEditor, registration)
{
	// The same cross for both, pointing the way each one pushes
	registration.RegisterCustomPropertyDrawer(ui::Margin::GetReflectionDescriptor(),
	                                          DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Outward));
	registration.RegisterCustomPropertyDrawer(ui::Padding::GetReflectionDescriptor(),
	                                          DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Inward));

	registration.RegisterCustomPropertyDrawer<ui::NodeRefBase, NodeRefCustomPropertyDrawer>();

	if (const EnumDescriptor* easingEnum = GetEasingEnumDescriptor())
	{
		registration.RegisterEnumValuePreview(*easingEnum, +[](uint64_t value) { DrawEasingPreview((ui::AnimationKey::Easing)value); });
	}

	registration.RegisterAssetType<UIPrefabAssetType>();
	registration.RegisterCustomComponentDrawer<ui::UIComponent, UIComponentCustomComponentDrawer>();

	if (ui::CanvasManager* canvasManager = ui::CanvasManager::GetInstance())
	{
		canvasManager->SetPointerSource(ui::CanvasManager::PointerSource::Manual);
	}
	ViewportGamePointer::Register(&GiveGamePointerToCanvases);

	return 0;
}

HOD_SHUTDOWN_EDITOR_MODULE(UIEditor)
{
	ViewportGamePointer::Unregister(&GiveGamePointerToCanvases);
	if (ui::CanvasManager* canvasManager = ui::CanvasManager::GetInstance())
	{
		canvasManager->SetPointerSource(ui::CanvasManager::PointerSource::Window);
	}
	return 0;
}
