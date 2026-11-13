#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorRegistrar.hpp"
#include "HodEngine/UIEditor/EasingPreview.hpp"
#include "HodEngine/UIEditor/CustomPropertyDrawer/NodeRefCustomPropertyDrawer.hpp"
#include "HodEngine/UIEditor/CustomPropertyDrawer/RectOffsetsCustomPropertyDrawer.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabAssetType.hpp"
#include "HodEngine/UIEditor/CustomComponentDrawer/UIComponentCustomComponentDrawer.hpp"

#include <HodEngine/GameEditor/GameEditorRegistrar.hpp>

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/UI/AnimationClip.hpp>
#include <HodEngine/UI/CanvasManager.hpp>
#include <HodEngine/UIComponents/Components/UIComponent.hpp>
#include <HodEngine/UI/Margin.hpp>
#include <HodEngine/UI/NodeRef.hpp>
#include <HodEngine/UI/Padding.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline ui
{
	namespace
	{
		/// @brief The easing enum as UI registered it on its properties (a Button's TransitionEasing...): each module (DLL) has its
		/// own instance of an enum's descriptor, this one's is UI's.
		const EnumDescriptor* GetEasingEnumDescriptor()
		{
			ReflectionPropertyVariable* property = AnimationKey::GetReflectionDescriptor().FindProperty<ReflectionPropertyVariable>("Easing");
			return (property != nullptr) ? property->GetEnumDescriptor() : nullptr;
		}

		/// @brief Playing, the game is drawn in the viewports: its canvases get the pointer from there, not from the main window
		void GiveGamePointerToCanvases(const Vector2& position, const Vector2& resolution, bool pressed, const Vector2& scroll)
		{
			if (CanvasManager* canvasManager = CanvasManager::GetInstance())
			{
				canvasManager->SetPointer(position, resolution, pressed, scroll);
			}
		}
	}

	HOD_STARTUP_EDITOR_MODULE(UIEditor, registration)
	{
		EditorRegistrar editorRegistrar(registration);
		// The same cross for both, pointing the way each one pushes
		editorRegistrar.RegisterCustomPropertyDrawer(Margin::GetReflectionDescriptor(),
		                                             DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Outward));
		editorRegistrar.RegisterCustomPropertyDrawer(Padding::GetReflectionDescriptor(),
		                                             DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Inward));

		editorRegistrar.RegisterCustomPropertyDrawer<NodeRefBase, NodeRefCustomPropertyDrawer>();

		if (const EnumDescriptor* easingEnum = GetEasingEnumDescriptor())
		{
			editorRegistrar.RegisterEnumValuePreview(*easingEnum, +[](uint64_t value) { DrawEasingPreview((AnimationKey::Easing)value); });
		}

		editorRegistrar.RegisterAssetType<UIPrefabAssetType>();
		editorRegistrar.RegisterCustomComponentDrawer<UIComponent, UIComponentCustomComponentDrawer>();

		// The canvases get the game pointer from the viewports, not from the window (the game is drawn in the viewports)
		if (CanvasManager* canvasManager = CanvasManager::GetInstance())
		{
			canvasManager->SetPointerSource(CanvasManager::PointerSource::Manual);
			registration.AddUnregister(
				[]()
				{
					if (CanvasManager* canvasManager = CanvasManager::GetInstance())
					{
						canvasManager->SetPointerSource(CanvasManager::PointerSource::Window);
					}
				});
		}
		GameEditorRegistrar gameEditorRegistrar(registration);
		gameEditorRegistrar.RegisterViewportGamePointer(&GiveGamePointerToCanvases);

		return 0;
	}
}
