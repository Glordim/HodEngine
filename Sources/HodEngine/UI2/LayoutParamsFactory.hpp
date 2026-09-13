#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include "HodEngine/UI2/LayoutParams.hpp"

#include <cstdint>
#include <map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline ui2
{
	/// @brief Registry of concrete LayoutParams types, keyed by reflection type hash, so a
	/// serialized Node tree can recreate the right concrete LayoutParams subtype for each child
	/// (see Node::DeserializeFromDocument). Mirrors Game::ComponentFactory.
	class HOD_UI2_API LayoutParamsFactory
	{
		_Singleton(LayoutParamsFactory)

	public:

		template<typename _LayoutParams_>
		bool Register();

		template<typename _LayoutParams_>
		bool Unregister();

		const std::map<uint64_t, ReflectionDescriptor*>& GetAllDescriptors() const { return _typeToDescriptors; }

	protected:

		~LayoutParamsFactory() = default;

	private:

		std::map<uint64_t, ReflectionDescriptor*> _typeToDescriptors;
	};
}

#include "LayoutParamsFactory.inl"
