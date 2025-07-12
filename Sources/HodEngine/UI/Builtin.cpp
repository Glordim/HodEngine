#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Builtin.hpp"

#include "HodEngine/Game/ComponentFactory.hpp"

#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Node.hpp"

#include "HodEngine/UI/Drawables/Texture.hpp"
#include "HodEngine/UI/Drawables/Text.hpp"

#include "HodEngine/UI/Layout/LayoutElement.hpp"

namespace hod::ui
{
	void RegisterBuiltin()
	{
		game::ComponentFactory* componentFactory = game::ComponentFactory::GetInstance();
		componentFactory->Register<Canvas>();
		componentFactory->Register<Node>();

		componentFactory->Register<Texture>();
		componentFactory->Register<Text>();

		componentFactory->Register<LayoutElement>();
	}
}
