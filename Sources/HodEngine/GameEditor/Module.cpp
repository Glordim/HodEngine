#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/Trait/ReflectionTraitCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"

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
	game::ZOrder::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(DefaultAllocator::GetInstance().New<ZOrderCustomEditor>());
	game::WeakComponentBase::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(DefaultAllocator::GetInstance().New<WeakComponentCustomEditor>());
	
	game::Node2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<Node2dComponentCustomEditor>());
	game::CameraComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<CameraComponentCustomEditor>());
	game::TextureRendererComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<TextureRendererComponentCustomEditor>());
	game::BoxCollider2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<BoxCollider2dComponentCustomEditor>());
	game::CircleCollider2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<CircleCollider2dComponentCustomEditor>());
	game::CapsuleCollider2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<CapsuleCollider2dComponentCustomEditor>());
	game::EdgeCollider2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<EdgeCollider2dComponentCustomEditor>());
	game::MultiShapeCollider2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<MultiShapeCollider2dComponentCustomEditor>());
	
	return 0;
}

HOD_SHUTDOWN_MODULE(GameEditor)
{
	game::ZOrder::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
	game::WeakComponentBase::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
	
	game::Node2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
	game::CameraComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
	game::TextureRendererComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
	game::BoxCollider2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
	game::CircleCollider2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
	game::CapsuleCollider2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
	game::EdgeCollider2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
	game::MultiShapeCollider2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();

	return 0;
}
