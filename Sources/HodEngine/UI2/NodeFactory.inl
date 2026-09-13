
namespace hod::inline ui2
{
	template<typename _Node_>
	bool NodeFactory::Register()
	{
		static_assert(std::is_base_of<Node, _Node_>::value, "_Node_ must derive from Node to use NodeFactory::Register()");

		_typeToDescriptors[_Node_::GetReflectionDescriptor().GetType()] = &_Node_::GetReflectionDescriptor();
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
