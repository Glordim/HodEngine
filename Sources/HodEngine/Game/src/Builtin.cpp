#include "Builtin.h"

#include <ActorReflection.h>
#include <ComponentReflection.h>

#include "Components/CameraComponent.h"
#include "Components/ColliderComponent.h"
#include "Components/RendererComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Light/PointLightComponent.h"
#include "Components/Light/DirLightComponent.h"
#include "Components/Light/SpotLightComponent.h"

namespace HOD
{
	namespace GAME
	{
		void RegisterBuiltin()
		{
			GAME::ComponentReflection::CreateInstance();
			GAME::ActorReflection::CreateInstance();

			ActorReflection* actorReflection = ActorReflection::GetInstance();
			actorReflection->Register<Actor>();

			ComponentReflection* componentReflection = ComponentReflection::GetInstance();
			componentReflection->Register<Component>();
			componentReflection->Register<SceneComponent>();
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