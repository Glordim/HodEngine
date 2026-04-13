#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"

#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UIEditor/ComponentCustomEditor/CanvasCustomEditor.hpp"
#include "HodEngine/UIEditor/ComponentCustomEditor/NodeCustomEditor.hpp"

#include "HodEngine/UI/Padding.hpp"
#include "HodEngine/UIEditor/PropertyCustomEditor/PaddingCustomEditor.hpp"

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawerRegistry.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::game;
using namespace hod::editor;

HOD_STARTUP_MODULE(UIEditor)
{
	ui::Canvas::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<CanvasCustomEditor>());
	ui::Node::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<NodeCustomEditor>());

	hod::editor::CustomPropertyDrawerRegistry::Register<ui::Padding, PaddingCustomEditor>();
	
	return 0;
}

HOD_SHUTDOWN_MODULE(UIEditor)
{
	ui::Canvas::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
	ui::Node::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();

	hod::editor::CustomPropertyDrawerRegistry::Unregister<ui::Padding>();

	return 0;
}
