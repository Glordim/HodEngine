#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/ComponentCustomEditor/CustomComponentDrawerRegistry.hpp"

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
	hod::editor::CustomComponentDrawerRegistry::Register<ui::Canvas, CanvasCustomEditor>();
	hod::editor::CustomComponentDrawerRegistry::Register<ui::Node, NodeCustomEditor>();

	hod::editor::CustomPropertyDrawerRegistry::Register<ui::Padding, PaddingCustomEditor>();
	
	return 0;
}

HOD_SHUTDOWN_MODULE(UIEditor)
{
	hod::editor::CustomComponentDrawerRegistry::Unregister<ui::Canvas>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<ui::Node>();

	hod::editor::CustomPropertyDrawerRegistry::Unregister<ui::Padding>();

	return 0;
}
