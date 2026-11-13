#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Builtin.hpp"

#include "HodEngine/UI/UIRegistrar.hpp"

#include "HodEngine/UI/LayoutParamsFactory.hpp"
#include "HodEngine/UI/AnchoredLayoutParams.hpp"
#include "HodEngine/UI/BoxLayoutParams.hpp"

#include "HodEngine/UI/NodeFactory.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/ImageNode.hpp"
#include "HodEngine/UI/TextNode.hpp"
#include "HodEngine/UI/MaskNode.hpp"
#include "HodEngine/UI/ShapeNode.hpp"

#include "HodEngine/UI/LayoutFactory.hpp"
#include "HodEngine/UI/BoxLayout.hpp"

#include "HodEngine/UI/InteractionFactory.hpp"
#include "HodEngine/UI/Button.hpp"
#include "HodEngine/UI/ScrollView.hpp"
#include "HodEngine/UI/Slider.hpp"
#include "HodEngine/UI/Toggle.hpp"

#include "HodEngine/UI/UISettings.hpp"

#include <HodEngine/GameSystems/GameSystemsRegistrar.hpp>

namespace hod::inline ui
{
	/// @brief
	/// @param registration
	void RegisterBuiltin(ModuleRegistration& registration)
	{
		UIRegistrar uiRegistrar(registration);
		uiRegistrar.RegisterLayoutParams<AnchoredLayoutParams>();
		uiRegistrar.RegisterLayoutParams<HorizontalLayoutParams>();
		uiRegistrar.RegisterLayoutParams<VerticalLayoutParams>();

		uiRegistrar.RegisterNode<Node>();
		uiRegistrar.RegisterNode<ImageNode>();
		uiRegistrar.RegisterNode<TextNode>();
		uiRegistrar.RegisterNode<MaskNode>();
		uiRegistrar.RegisterNode<ShapeNode>();

		uiRegistrar.RegisterLayout<HorizontalLayout>();
		uiRegistrar.RegisterLayout<VerticalLayout>();

		uiRegistrar.RegisterInteraction<Button>();
		uiRegistrar.RegisterInteraction<Toggle>();
		uiRegistrar.RegisterInteraction<Slider>();
		uiRegistrar.RegisterInteraction<ScrollView>();

		GameSystemsRegistrar gameSystemsRegistrar(registration);
		gameSystemsRegistrar.RegisterProjectSettings<UISettings>();
	}
}
