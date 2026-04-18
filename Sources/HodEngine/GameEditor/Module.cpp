#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawerRegistry.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawerRegistry.hpp"

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

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::game;
using namespace hod::editor;

HOD_STARTUP_MODULE(GameEditor)
{
	hod::editor::CustomPropertyDrawerRegistry::Register<ZOrder, ZOrderCustomPropertyDrawer>();
	hod::editor::CustomPropertyDrawerRegistry::Register<WeakComponentBase, WeakComponentCustomPropertyDrawer>();
	
	hod::editor::CustomComponentDrawerRegistry::Register<Node2dComponent, Node2dCustomComponentDrawer>();
	hod::editor::CustomComponentDrawerRegistry::Register<CameraComponent, CameraCustomComponentDrawer>();
	hod::editor::CustomComponentDrawerRegistry::Register<TextureRendererComponent, TextureRendererCustomComponentDrawer>();
	hod::editor::CustomComponentDrawerRegistry::Register<BoxCollider2dComponent, BoxCollider2dCustomComponentDrawer>();
	hod::editor::CustomComponentDrawerRegistry::Register<CircleCollider2dComponent, CircleCollider2dCustomComponentDrawer>();
	hod::editor::CustomComponentDrawerRegistry::Register<CapsuleCollider2dComponent, CapsuleCollider2dCustomComponentDrawer>();
	hod::editor::CustomComponentDrawerRegistry::Register<EdgeCollider2dComponent, EdgeCollider2dCustomComponentDrawer>();
	hod::editor::CustomComponentDrawerRegistry::Register<MultiShapeCollider2dComponent, MultiShapeCollider2dCustomComponentDrawer>();
	
	return 0;
}

HOD_SHUTDOWN_MODULE(GameEditor)
{
	hod::editor::CustomPropertyDrawerRegistry::Unregister<ZOrder>();
	hod::editor::CustomPropertyDrawerRegistry::Unregister<WeakComponentBase>();
	
	hod::editor::CustomComponentDrawerRegistry::Unregister<Node2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<CameraComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<TextureRendererComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<BoxCollider2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<CircleCollider2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<CapsuleCollider2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<EdgeCollider2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<MultiShapeCollider2dComponent>();

	return 0;
}
