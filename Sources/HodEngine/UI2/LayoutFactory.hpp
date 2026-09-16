#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include "HodEngine/UI2/Layout.hpp"

#include <cstdint>
#include <map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline ui2
{
	/// @brief Registry of concrete Layout types, keyed by reflection type hash, so a serialized Node
	/// can recreate the right concrete Layout subtype for its own attached layout (see
	/// Node::DeserializeFromDocument), and so editor tooling can eventually list every attachable
	/// Layout type without hardcoding them. Mirrors LayoutParamsFactory / NodeFactory / Game::ComponentFactory.
	class HOD_UI2_API LayoutFactory
	{
		_Singleton(LayoutFactory)

	public:

		template<typename _Layout_>
		bool Register();

		template<typename _Layout_>
		bool Unregister();

		const std::map<uint64_t, ReflectionDescriptor*>& GetAllDescriptors() const { return _typeToDescriptors; }

	protected:

		~LayoutFactory() = default;

	private:

		std::map<uint64_t, ReflectionDescriptor*> _typeToDescriptors;
	};
}

#include "LayoutFactory.inl"
