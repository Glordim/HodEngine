#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Builtin.hpp"

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

#include <HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp>

namespace hod::inline ui
{
	void RegisterBuiltin()
	{
		LayoutParamsFactory* layoutParamsFactory = LayoutParamsFactory::GetInstance();
		layoutParamsFactory->Register<AnchoredLayoutParams>();
		layoutParamsFactory->Register<HorizontalLayoutParams>();
		layoutParamsFactory->Register<VerticalLayoutParams>();

		NodeFactory* nodeFactory = NodeFactory::GetInstance();
		nodeFactory->Register<Node>();
		nodeFactory->Register<ImageNode>();
		nodeFactory->Register<TextNode>();
		nodeFactory->Register<MaskNode>();
		nodeFactory->Register<ShapeNode>();

		LayoutFactory* layoutFactory = LayoutFactory::GetInstance();
		layoutFactory->Register<HorizontalLayout>();
		layoutFactory->Register<VerticalLayout>();

		InteractionFactory* interactionFactory = InteractionFactory::GetInstance();
		interactionFactory->Register<Button>();
		interactionFactory->Register<Toggle>();
		interactionFactory->Register<Slider>();
		interactionFactory->Register<ScrollView>();

		ProjectSettingsManager::GetInstance()->Register<UISettings>();
	}
}
