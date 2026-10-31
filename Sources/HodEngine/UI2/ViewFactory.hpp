#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include "HodEngine/UI2/View.hpp"

#include <cstdint>
#include <map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline ui2
{
	/// @brief Registry of concrete View types, keyed by reflection type hash, so a serialized Node can recreate
	/// its View (see Node::DeserializeFromDocument) and the editor can list the attachable ones. Mirrors LayoutFactory.
	class HOD_UI2_API ViewFactory
	{
		_Singleton(ViewFactory)

	public:

		template<typename _View_>
		bool Register();

		template<typename _View_>
		bool Unregister();

		const std::map<uint64_t, ReflectionDescriptor*>& GetAllDescriptors() const { return _typeToDescriptors; }

	protected:

		~ViewFactory() = default;

	private:

		std::map<uint64_t, ReflectionDescriptor*> _typeToDescriptors;
	};
}

#include "ViewFactory.inl"
