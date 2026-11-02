#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/UI2Editor/EasingPreview.hpp"
#include "HodEngine/UI2Editor/CustomPropertyDrawer/NodeRefCustomPropertyDrawer.hpp"
#include "HodEngine/UI2Editor/CustomPropertyDrawer/RectOffsetsCustomPropertyDrawer.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabAssetType.hpp"


#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/UI2/AnimationClip.hpp>
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
		PropertyDrawer::SetEnumValuePreview(*easingEnum, +[](uint64_t value) { DrawEasingPreview((ui2::AnimationKey::Easing)value); });
	}

	registration.RegisterAssetType<UIPrefabAssetType>();

	return 0;
}

HOD_SHUTDOWN_EDITOR_MODULE(UI2Editor)
{
	if (const EnumDescriptor* easingEnum = GetEasingEnumDescriptor())
	{
		PropertyDrawer::SetEnumValuePreview(*easingEnum, nullptr);
	}
	return 0;
}
