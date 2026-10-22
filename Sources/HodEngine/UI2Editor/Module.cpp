#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawerRegistry.hpp"
#include "HodEngine/UI2Editor/CustomPropertyDrawer/RectOffsetsCustomPropertyDrawer.hpp"

#include <HodEngine/UI2/Margin.hpp>
#include <HodEngine/UI2/Padding.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

HOD_STARTUP_MODULE(UI2Editor)
{
	// The same cross for both, pointing the way each one pushes
	hod::CustomPropertyDrawerRegistry::Register(ui2::Margin::GetReflectionDescriptor(),
	                                            DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Outward));
	hod::CustomPropertyDrawerRegistry::Register(ui2::Padding::GetReflectionDescriptor(),
	                                            DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Inward));
	return 0;
}

HOD_SHUTDOWN_MODULE(UI2Editor)
{
	hod::CustomPropertyDrawerRegistry::Unregister<ui2::Margin>();
	hod::CustomPropertyDrawerRegistry::Unregister<ui2::Padding>();
	return 0;
}
