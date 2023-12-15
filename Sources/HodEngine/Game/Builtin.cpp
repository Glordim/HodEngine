#include "HodEngine/Game/Builtin.hpp"

#include "HodEngine/Game/ActorReflection.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

#include "HodEngine/Game/Components/CameraComponent.hpp"
#include "HodEngine/Game/Components/ColliderComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/Components/SpriteComponent.hpp"
#include "HodEngine/Game/Components/Light/PointLightComponent.hpp"
#include "HodEngine/Game/Components/Light/DirLightComponent.hpp"
#include "HodEngine/Game/Components/Light/SpotLightComponent.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"

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