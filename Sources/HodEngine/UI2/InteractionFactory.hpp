#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include "HodEngine/UI2/Interaction.hpp"

#include <cstdint>
#include <map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline ui2
{
	/// @brief Registry of concrete Interaction types, keyed by reflection type hash, so a serialized Node can recreate
	/// its Interaction (see Node::DeserializeFromDocument) and the editor can list the attachable ones. Mirrors LayoutFactory.
	class HOD_UI2_API InteractionFactory
	{
		_Singleton(InteractionFactory)

	public:

		template<typename _Interaction_>
		bool Register();

		template<typename _Interaction_>
		bool Unregister();

		const std::map<uint64_t, ReflectionDescriptor*>& GetAllDescriptors() const { return _typeToDescriptors; }

	protected:

		~InteractionFactory() = default;

	private:

		std::map<uint64_t, ReflectionDescriptor*> _typeToDescriptors;
	};
}

#include "InteractionFactory.inl"
