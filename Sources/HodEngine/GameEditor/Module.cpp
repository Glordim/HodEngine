#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorRegistrar.hpp"

#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Game/Components/Node2dComponent.hpp"

#include "HodEngine/Game/Components/MissingComponent.hpp"

#include "HodEngine/GameEditor/CustomPropertyDrawer/ZOrderCustomPropertyDrawer.hpp"
#include "HodEngine/GameEditor/CustomPropertyDrawer/WeakComponentCustomPropertyDrawer.hpp"

#include "HodEngine/GameEditor/CustomComponentDrawer/Node2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/MissingComponentCustomComponentDrawer.hpp"

#include "HodEngine/GameEditor/DataItemEditor/DataItemAssetType.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableAssetType.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabAssetType.hpp"
#include "HodEngine/GameEditor/SceneEditor/SceneAssetType.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::game;
using namespace hod::editor;

HOD_STARTUP_EDITOR_MODULE(GameEditor, registration)
{
	hod::editor::EditorRegistrar editorRegistrar(registration);
	editorRegistrar.RegisterCustomPropertyDrawer<ZOrder, ZOrderCustomPropertyDrawer>();
	editorRegistrar.RegisterCustomPropertyDrawer<WeakComponentBase, WeakComponentCustomPropertyDrawer>();

	editorRegistrar.RegisterCustomComponentDrawer<Node2dComponent, Node2dCustomComponentDrawer>();
	editorRegistrar.RegisterCustomComponentDrawer<MissingComponent, MissingComponentCustomComponentDrawer>();

	editorRegistrar.RegisterAssetType<SceneAssetType>();
	editorRegistrar.RegisterAssetType<PrefabAssetType>();
	editorRegistrar.RegisterAssetType<DataItemAssetType>();
	editorRegistrar.RegisterAssetType<DataTableAssetType>();
	return 0;
}
