#include "HodEngine/GameEditor/Pch.hpp"
#include <HodEngine/Core/Module.hpp>
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorRegistrar.hpp"

#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Game/Components/Node2dComponent.hpp"

#include "HodEngine/Game/Components/MissingComponent.hpp"

#include "HodEngine/GameEditor/PropertyDrawer/ZOrderDrawer.hpp"
#include "HodEngine/GameEditor/PropertyDrawer/WeakComponentDrawer.hpp"

#include "HodEngine/GameEditor/ComponentGizmo/Node2dGizmo.hpp"
#include "HodEngine/GameEditor/ComponentInspector/MissingComponentInspector.hpp"
#include "HodEngine/GameEditor/GameEditorRegistrar.hpp"

#include "HodEngine/GameEditor/DataItemEditor/DataItemAssetType.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableAssetType.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabAssetType.hpp"
#include "HodEngine/GameEditor/SceneEditor/SceneAssetType.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline game
{
	HOD_STARTUP_MODULE(GameEditor, registration)
	{
		EditorRegistrar editorRegistrar(registration);
		editorRegistrar.RegisterPropertyDrawer<ZOrderDrawer>();
		editorRegistrar.RegisterPropertyDrawer<WeakComponentDrawer>();

		GameEditorRegistrar gameEditorRegistrar(registration);
		gameEditorRegistrar.RegisterComponentGizmo<Node2dGizmo>();
		gameEditorRegistrar.RegisterComponentInspector<MissingComponentInspector>();

		editorRegistrar.RegisterAssetType<SceneAssetType>();
		editorRegistrar.RegisterAssetType<PrefabAssetType>();
		editorRegistrar.RegisterAssetType<DataItemAssetType>();
		editorRegistrar.RegisterAssetType<DataTableAssetType>();
		return true;
	}
}
