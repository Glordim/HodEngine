#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Builtin.hpp"

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

#include "HodEngine/Game/Components/Primitive/BoxComponent.hpp"


namespace hod::inline game
{
	void RegisterBuiltin()
	{
		ComponentFactory* componentFactory = ComponentFactory::GetInstance();
		componentFactory->Register<NodeComponent>();
		componentFactory->Register<Node2dComponent>();
		componentFactory->Register<BoxComponent>();
		componentFactory->Register<TextureRendererComponent>();
		componentFactory->Register<TextRendererComponent>();
		componentFactory->Register<CameraComponent>();
		//componentFactory->Register<SpriteRendererComponent>();
		/*
		componentReflection->Register<ColliderComponent>();
		componentReflection->Register<RendererComponent>();
		componentReflection->Register<SpriteRendererComponent>();
		componentReflection->Register<PointLightComponent>();
		componentReflection->Register<DirLightComponent>();
		componentReflection->Register<SpotLightComponent>();
		*/
	}
}