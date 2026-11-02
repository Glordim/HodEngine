#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorModuleRegistration.hpp"

#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Components/CameraComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/Components/SpriteRendererComponent.hpp"
#include "HodEngine/Game/Components/TextureRendererComponent.hpp"
#include "HodEngine/Game/Components/UIComponent.hpp"
#include "HodEngine/Game/Components/TextRendererComponent.hpp"
#include "HodEngine/Game/Components/Light/PointLightComponent.hpp"
#include "HodEngine/Game/Components/Light/DirLightComponent.hpp"
#include "HodEngine/Game/Components/Light/SpotLightComponent.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"

#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/MultiShapeCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/BoxCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/CircleCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/CapsuleCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/EdgeCollider2dComponent.hpp"

#include "HodEngine/Game/Components/Primitive/BoxComponent.hpp"

#include "HodEngine/GameEditor/CustomPropertyDrawer/ZOrderCustomPropertyDrawer.hpp"
#include "HodEngine/GameEditor/CustomPropertyDrawer/WeakComponentCustomPropertyDrawer.hpp"

#include "HodEngine/GameEditor/CustomComponentDrawer/Node2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/CameraCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/BoxCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/CircleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/CapsuleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/EdgeCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/MultiShapeCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/TextureRendererCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/UIComponentCustomComponentDrawer.hpp"

#include "HodEngine/GameEditor/DataItemEditor/DataItemAssetType.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableAssetType.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabAssetType.hpp"
#include "HodEngine/GameEditor/SceneEditor/SceneAssetType.hpp"



#include <HodEngine/UI2/CanvasManager.hpp>

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
	registration.RegisterCustomComponentDrawer<UIComponent, UIComponentCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<BoxCollider2dComponent, BoxCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<CircleCollider2dComponent, CircleCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<CapsuleCollider2dComponent, CapsuleCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<EdgeCollider2dComponent, EdgeCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<MultiShapeCollider2dComponent, MultiShapeCollider2dCustomComponentDrawer>();

	registration.RegisterAssetType<SceneAssetType>();
	registration.RegisterAssetType<PrefabAssetType>();
	registration.RegisterAssetType<DataItemAssetType>();
	registration.RegisterAssetType<DataTableAssetType>();

	// The game is drawn in the viewports, which give the game's canvases the pointer (see ViewportWindow), not the main window
	if (ui2::CanvasManager* canvasManager = ui2::CanvasManager::GetInstance())
	{
		canvasManager->SetPointerSource(ui2::CanvasManager::PointerSource::Manual);
	}
	return 0;
}

HOD_SHUTDOWN_EDITOR_MODULE(GameEditor)
{
	if (ui2::CanvasManager* canvasManager = ui2::CanvasManager::GetInstance())
	{
		canvasManager->SetPointerSource(ui2::CanvasManager::PointerSource::Window);
	}
	return 0;
}
