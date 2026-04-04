#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Math/Vector4.hpp"
#include "HodEngine/Core/Resource/WeakResource.hpp"

#include "HodEngine/CoreEditor/PropertyCustomEditor/ColorCustomEditor.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/Vector2CustomEditor.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/Vector4CustomEditor.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/WeakResourceCustomEditor.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitCustomPropertyDrawer.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

HOD_STARTUP_MODULE(CoreEditor)
{
	math::Color::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(DefaultAllocator::GetInstance().New<ColorCustomEditor>());
	math::Vector2::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(DefaultAllocator::GetInstance().New<Vector2CustomEditor>());
	math::Vector4::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(DefaultAllocator::GetInstance().New<Vector4CustomEditor>());
	WeakResourceBase::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(DefaultAllocator::GetInstance().New<WeakResourceCustomEditor>());

	return 0;
}

HOD_SHUTDOWN_MODULE(CoreEditor)
{
	math::Color::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
	math::Vector2::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
	math::Vector4::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
	WeakResourceBase::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();

	return 0;
}
