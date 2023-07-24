#include "HodEngine/Game/Builtin.h"

#include "HodEngine/Game/ActorReflection.h"
#include "HodEngine/Game/ComponentFactory.h"

#include "HodEngine/Game/Components/CameraComponent.h"
#include "HodEngine/Game/Components/ColliderComponent.h"
#include "HodEngine/Game/Components/RendererComponent.h"
#include "HodEngine/Game/Components/SceneComponent.h"
#include "HodEngine/Game/Components/SpriteComponent.h"
#include "HodEngine/Game/Components/Light/PointLightComponent.h"
#include "HodEngine/Game/Components/Light/DirLightComponent.h"
#include "HodEngine/Game/Components/Light/SpotLightComponent.h"

#include "HodEngine/Game/Components/NodeComponent.h"
#include "HodEngine/Game/Components/Node2dComponent.h"

#include "HodEngine/Game/Components/Primitive/BoxComponent.h"

namespace hod
{
	namespace game
	{
		void RegisterBuiltin()
		{
			game::ComponentFactory::CreateInstance();
			game::ActorReflection::CreateInstance();

			ActorReflection* actorReflection = ActorReflection::GetInstance();
			//actorReflection->Register<Actor>();

			ComponentFactory* componentFactory = ComponentFactory::GetInstance();
			componentFactory->Register<NodeComponent>();
			componentFactory->Register<Node2dComponent>();
			componentFactory->Register<BoxComponent>();
			//componentFactory->Register<SpriteComponent>();
			/*
			componentReflection->Register<CameraComponent>();
			componentReflection->Register<ColliderComponent>();
			componentReflection->Register<RendererComponent>();
			componentReflection->Register<SpriteComponent>();
			componentReflection->Register<PointLightComponent>();
			componentReflection->Register<DirLightComponent>();
			componentReflection->Register<SpotLightComponent>();
			*/
		}
	}
}