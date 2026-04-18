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
	hod::CustomPropertyDrawerRegistry::Register<ZOrder, ZOrderCustomPropertyDrawer>();
	hod::CustomPropertyDrawerRegistry::Register<WeakComponentBase, WeakComponentCustomPropertyDrawer>();
	
	hod::CustomComponentDrawerRegistry::Register<Node2dComponent, Node2dCustomComponentDrawer>();
	hod::CustomComponentDrawerRegistry::Register<CameraComponent, CameraCustomComponentDrawer>();
	hod::CustomComponentDrawerRegistry::Register<TextureRendererComponent, TextureRendererCustomComponentDrawer>();
	hod::CustomComponentDrawerRegistry::Register<BoxCollider2dComponent, BoxCollider2dCustomComponentDrawer>();
	hod::CustomComponentDrawerRegistry::Register<CircleCollider2dComponent, CircleCollider2dCustomComponentDrawer>();
	hod::CustomComponentDrawerRegistry::Register<CapsuleCollider2dComponent, CapsuleCollider2dCustomComponentDrawer>();
	hod::CustomComponentDrawerRegistry::Register<EdgeCollider2dComponent, EdgeCollider2dCustomComponentDrawer>();
	hod::CustomComponentDrawerRegistry::Register<MultiShapeCollider2dComponent, MultiShapeCollider2dCustomComponentDrawer>();
	
	return 0;
}

HOD_SHUTDOWN_MODULE(GameEditor)
{
	hod::CustomPropertyDrawerRegistry::Unregister<ZOrder>();
	hod::CustomPropertyDrawerRegistry::Unregister<WeakComponentBase>();
	
	hod::CustomComponentDrawerRegistry::Unregister<Node2dComponent>();
	hod::CustomComponentDrawerRegistry::Unregister<CameraComponent>();
	hod::CustomComponentDrawerRegistry::Unregister<TextureRendererComponent>();
	hod::CustomComponentDrawerRegistry::Unregister<BoxCollider2dComponent>();
	hod::CustomComponentDrawerRegistry::Unregister<CircleCollider2dComponent>();
	hod::CustomComponentDrawerRegistry::Unregister<CapsuleCollider2dComponent>();
	hod::CustomComponentDrawerRegistry::Unregister<EdgeCollider2dComponent>();
	hod::CustomComponentDrawerRegistry::Unregister<MultiShapeCollider2dComponent>();

	return 0;
}
