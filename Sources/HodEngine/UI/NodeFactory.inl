#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui
{
	/// @return false (with an error) if registered already: the first one stays
	template<typename _Node_>
	bool NodeFactory::Register()
	{
		static_assert(std::is_base_of<Node, _Node_>::value, "_Node_ must derive from Node to use NodeFactory::Register()");

		ReflectionDescriptor& descriptor = _Node_::GetReflectionDescriptor();
		if (_typeToDescriptors.try_emplace(descriptor.GetType(), &descriptor).second == false)
		{
			OUTPUT_ERROR("NodeFactory: the node '{}' is registered already", descriptor.GetDisplayName());
			return false;
		}
		return true;
	}

	template<typename _Node_>
	bool NodeFactory::Unregister()
	{
		static_assert(std::is_base_of<Node, _Node_>::value, "_Node_ must derive from Node to use NodeFactory::Unregister()");

		auto it = _typeToDescriptors.find(_Node_::GetReflectionDescriptor().GetType());
		if (it != _typeToDescriptors.end())
		{
			_typeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}
