#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/Padding.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawerRegistry.hpp"
#include "HodEngine/UIEditor/CustomComponentDrawer/CanvasCustomComponentDrawer.hpp"
#include "HodEngine/UIEditor/CustomComponentDrawer/NodeCustomComponentDrawer.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawerRegistry.hpp"
#include "HodEngine/UIEditor/CustomPropertyDrawer/PaddingCustomPropertyDrawer.hpp"


REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::game;
using namespace hod::editor;

HOD_STARTUP_MODULE(UIEditor)
{
	hod::CustomComponentDrawerRegistry::Register<ui::Canvas, CanvasCustomComponentDrawer>();
	hod::CustomComponentDrawerRegistry::Register<ui::Node, NodeCustomComponentDrawer>();

	hod::CustomPropertyDrawerRegistry::Register<ui::Padding, PaddingCustomPropertyDrawer>();
	
	return 0;
}

HOD_SHUTDOWN_MODULE(UIEditor)
{
	hod::CustomComponentDrawerRegistry::Unregister<ui::Canvas>();
	hod::CustomComponentDrawerRegistry::Unregister<ui::Node>();

	hod::CustomPropertyDrawerRegistry::Unregister<ui::Padding>();

	return 0;
}
