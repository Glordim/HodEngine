#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include "HodEngine/UI2/Node.hpp"

#include <cstdint>
#include <map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline ui2
{
	/// @brief Registry of concrete Node types, keyed by reflection type hash, so editor tooling
	/// (the UIPrefabEditor Library window) can list and instantiate every addable Node type
	/// without hard-coding them. Mirrors LayoutParamsFactory / Game::ComponentFactory.
	class HOD_UI2_API NodeFactory
	{
		_Singleton(NodeFactory)

	public:

		template<typename _Node_>
		bool Register();

		template<typename _Node_>
		bool Unregister();

		const std::map<uint64_t, ReflectionDescriptor*>& GetAllDescriptors() const { return _typeToDescriptors; }

	protected:

		~NodeFactory() = default;

	private:

		std::map<uint64_t, ReflectionDescriptor*> _typeToDescriptors;
	};
}

#include "NodeFactory.inl"
