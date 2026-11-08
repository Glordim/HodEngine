#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorModuleRegistration.hpp"

#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Components/CameraComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/Components/SpriteRendererComponent.hpp"
#include "HodEngine/Game/Components/TextureRendererComponent.hpp"
#include "HodEngine/Game/Components/TextRendererComponent.hpp"
#include "HodEngine/Game/Components/Light/PointLightComponent.hpp"
#include "HodEngine/Game/Components/Light/DirLightComponent.hpp"
#include "HodEngine/Game/Components/Light/SpotLightComponent.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"

#include "HodEngine/Game/Components/MissingComponent.hpp"

#include "HodEngine/Game/Components/Primitive/BoxComponent.hpp"

#include "HodEngine/GameEditor/CustomPropertyDrawer/ZOrderCustomPropertyDrawer.hpp"
#include "HodEngine/GameEditor/CustomPropertyDrawer/WeakComponentCustomPropertyDrawer.hpp"

#include "HodEngine/GameEditor/CustomComponentDrawer/Node2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/CameraCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/MissingComponentCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/TextureRendererCustomComponentDrawer.hpp"

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
	registration.RegisterCustomPropertyDrawer<ZOrder, ZOrderCustomPropertyDrawer>();
	registration.RegisterCustomPropertyDrawer<WeakComponentBase, WeakComponentCustomPropertyDrawer>();

	registration.RegisterCustomComponentDrawer<Node2dComponent, Node2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<CameraComponent, CameraCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<TextureRendererComponent, TextureRendererCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<MissingComponent, MissingComponentCustomComponentDrawer>();

	registration.RegisterAssetType<SceneAssetType>();
	registration.RegisterAssetType<PrefabAssetType>();
	registration.RegisterAssetType<DataItemAssetType>();
	registration.RegisterAssetType<DataTableAssetType>();
	return 0;
}
