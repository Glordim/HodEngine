#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/Builtin.hpp"

#include "HodEngine/UI2/LayoutParamsFactory.hpp"
#include "HodEngine/UI2/AnchoredLayoutParams.hpp"
#include "HodEngine/UI2/BoxLayoutParams.hpp"

#include "HodEngine/UI2/NodeFactory.hpp"
#include "HodEngine/UI2/Node.hpp"

#include "HodEngine/UI2/LayoutFactory.hpp"
#include "HodEngine/UI2/BoxLayout.hpp"

namespace hod::inline ui2
{
	void RegisterBuiltin()
	{
		LayoutParamsFactory* layoutParamsFactory = LayoutParamsFactory::GetInstance();
		layoutParamsFactory->Register<AnchoredLayoutParams>();
		layoutParamsFactory->Register<BoxLayoutParams>();

		NodeFactory* nodeFactory = NodeFactory::GetInstance();
		nodeFactory->Register<Node>();

		LayoutFactory* layoutFactory = LayoutFactory::GetInstance();
		layoutFactory->Register<BoxLayout>();
	}
}
