#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/LegacyPadding.hpp"

#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/UIEditor/CustomComponentDrawer/CanvasCustomComponentDrawer.hpp"
#include "HodEngine/UIEditor/CustomComponentDrawer/NodeCustomComponentDrawer.hpp"

#include "HodEngine/UIEditor/CustomPropertyDrawer/LegacyPaddingCustomPropertyDrawer.hpp"


REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::game;
using namespace hod::editor;

namespace
{
	EditorModuleRegistration registration;
}

HOD_STARTUP_MODULE(UIEditor)
{
	registration.RegisterCustomComponentDrawer<Canvas, CanvasCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<Node, NodeCustomComponentDrawer>();

	registration.RegisterCustomPropertyDrawer<LegacyPadding, LegacyPaddingCustomPropertyDrawer>();
	return 0;
}

HOD_SHUTDOWN_MODULE(UIEditor)
{
	registration.UnregisterAll();
	return 0;
}
