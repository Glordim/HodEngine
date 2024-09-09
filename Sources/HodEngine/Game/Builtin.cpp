#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Builtin.hpp"

#include "HodEngine/Game/ActorReflection.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

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
#include "HodEngine/Game/Components/Physics/2d/BoxCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/CircleCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/CapsuleCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/EdgeCollider2dComponent.hpp"

#include "HodEngine/Game/Components/Primitive/BoxComponent.hpp"

namespace hod
{
	namespace game
	{
		void RegisterBuiltin()
		{
			game::ComponentFactory::CreateInstance();
			game::ActorReflection::CreateInstance();

			ComponentFactory* componentFactory = ComponentFactory::GetInstance();
			componentFactory->Register<NodeComponent>();
			componentFactory->Register<Node2dComponent>();
			componentFactory->Register<BoxComponent>();
			componentFactory->Register<TextureRendererComponent>();
			componentFactory->Register<TextRendererComponent>();			
			componentFactory->Register<Collider2dComponent>();
			componentFactory->Register<BoxCollider2dComponent>();
			componentFactory->Register<CircleCollider2dComponent>();
			componentFactory->Register<CapsuleCollider2dComponent>();
			componentFactory->Register<EdgeCollider2dComponent>();
			componentFactory->Register<Rigidbody2dComponent>();
			//componentFactory->Register<SpriteRendererComponent>();
			/*
			componentReflection->Register<CameraComponent>();
			componentReflection->Register<ColliderComponent>();
			componentReflection->Register<RendererComponent>();
			componentReflection->Register<SpriteRendererComponent>();
			componentReflection->Register<PointLightComponent>();
			componentReflection->Register<DirLightComponent>();
			componentReflection->Register<SpotLightComponent>();
			*/
		}
	}
}