#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include "HodEngine/Core/TypeTrait.hpp"
#include "HodEngine/Game/DataStruct.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"

#include <map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline game
{
	/// @brief
	class HOD_GAME_API DataStructFactory
	{
		_Singleton(DataStructFactory)

	public:

		template<typename _DataStruct_>
		bool				Register();

		template<typename _DataStruct_>
		bool				Unregister();

		const std::map<RttiType, ReflectionDescriptor*>& GetAllDescriptors() const { return _RttiTypeToDescriptors; }
		ReflectionDescriptor* FindReflectionDescriptor(RttiType type) const;

	protected:

		~DataStructFactory() = default;

	private:

		std::map<RttiType, ReflectionDescriptor*>	_RttiTypeToDescriptors;
	};
}

#include "DataStructFactory.inl"
