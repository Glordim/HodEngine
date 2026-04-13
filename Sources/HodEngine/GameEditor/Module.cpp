#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawerRegistry.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/CustomComponentDrawerRegistry.hpp"

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

#include "HodEngine/GameEditor/PropertyCustomEditor/ZOrderCustomEditor.hpp"
#include "HodEngine/GameEditor/PropertyCustomEditor/WeakComponentCustomEditor.hpp"

#include "HodEngine/GameEditor/ComponentCustomEditor/Node2dComponentCustomEditor.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/CameraComponentCustomEditor.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/BoxCollider2dComponentCustomEditor.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/CircleCollider2dComponentCustomEditor.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/CapsuleCollider2dComponentCustomEditor.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/EdgeCollider2dComponentCustomEditor.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/MultiShapeCollider2dComponentCustomEditor.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/TextureRendererComponentCustomEditor.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::game;
using namespace hod::editor;

HOD_STARTUP_MODULE(GameEditor)
{
	hod::editor::CustomPropertyDrawerRegistry::Register<game::ZOrder, ZOrderCustomEditor>();
	hod::editor::CustomPropertyDrawerRegistry::Register<game::WeakComponentBase, WeakComponentCustomEditor>();
	
	hod::editor::CustomComponentDrawerRegistry::Register<game::Node2dComponent, Node2dComponentCustomEditor>();
	hod::editor::CustomComponentDrawerRegistry::Register<game::CameraComponent, CameraComponentCustomEditor>();
	hod::editor::CustomComponentDrawerRegistry::Register<game::TextureRendererComponent, TextureRendererComponentCustomEditor>();
	hod::editor::CustomComponentDrawerRegistry::Register<game::BoxCollider2dComponent, BoxCollider2dComponentCustomEditor>();
	hod::editor::CustomComponentDrawerRegistry::Register<game::CircleCollider2dComponent, CircleCollider2dComponentCustomEditor>();
	hod::editor::CustomComponentDrawerRegistry::Register<game::CapsuleCollider2dComponent, CapsuleCollider2dComponentCustomEditor>();
	hod::editor::CustomComponentDrawerRegistry::Register<game::EdgeCollider2dComponent, EdgeCollider2dComponentCustomEditor>();
	hod::editor::CustomComponentDrawerRegistry::Register<game::MultiShapeCollider2dComponent, MultiShapeCollider2dComponentCustomEditor>();
	
	return 0;
}

HOD_SHUTDOWN_MODULE(GameEditor)
{
	hod::editor::CustomPropertyDrawerRegistry::Unregister<game::ZOrder>();
	hod::editor::CustomPropertyDrawerRegistry::Unregister<game::WeakComponentBase>();
	
	hod::editor::CustomComponentDrawerRegistry::Unregister<game::Node2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<game::CameraComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<game::TextureRendererComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<game::BoxCollider2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<game::CircleCollider2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<game::CapsuleCollider2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<game::EdgeCollider2dComponent>();
	hod::editor::CustomComponentDrawerRegistry::Unregister<game::MultiShapeCollider2dComponent>();

	return 0;
}
