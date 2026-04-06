#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include "HodEngine/Core/TypeTrait.hpp"
#include "HodEngine/Game/SerializedData.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"

#include <map>

namespace hod::core
{
	class ReflectionDescriptor;
}

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API SerializedDataFactory
	{
		_Singleton(SerializedDataFactory)

	public:

		template<typename _SerializedData_>
		bool				Register();

		template<typename _SerializedData_>
		bool				Unregister();

		const std::map<RttiType, ReflectionDescriptor*>& GetAllDescriptors() const { return _RttiTypeToDescriptors; }
		ReflectionDescriptor* FindReflectionDescriptor(RttiType type) const;

	protected:

		~SerializedDataFactory() = default;

	private:

		std::map<RttiType, ReflectionDescriptor*>	_RttiTypeToDescriptors;
	};
}

#include "SerializedDataFactory.inl"
