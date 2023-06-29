#include "HodEngine/Game/src/Builtin.h"

#include "HodEngine/Game/src/ActorReflection.h"
#include "HodEngine/Game/src/ComponentFactory.h"

#include "HodEngine/Game/src/Components/CameraComponent.h"
#include "HodEngine/Game/src/Components/ColliderComponent.h"
#include "HodEngine/Game/src/Components/RendererComponent.h"
#include "HodEngine/Game/src/Components/SceneComponent.h"
#include "HodEngine/Game/src/Components/SpriteComponent.h"
#include "HodEngine/Game/src/Components/Light/PointLightComponent.h"
#include "HodEngine/Game/src/Components/Light/DirLightComponent.h"
#include "HodEngine/Game/src/Components/Light/SpotLightComponent.h"

#include "HodEngine/Game/src/Components/NodeComponent.h"
#include "HodEngine/Game/src/Components/Node2dComponent.h"

namespace hod
{
	namespace game
	{
		void RegisterBuiltin()
		{
			game::ComponentFactory::CreateInstance();
			game::ActorReflection::CreateInstance();

			ActorReflection* actorReflection = ActorReflection::GetInstance();
			actorReflection->Register<Actor>();

			ComponentFactory* componentFactory = ComponentFactory::GetInstance();
			componentFactory->Register<NodeComponent>();
			componentFactory->Register<Node2dComponent>();
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