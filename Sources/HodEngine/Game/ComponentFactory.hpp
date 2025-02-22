#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include "HodEngine/Core/Type.hpp"
#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"

#include <map>

namespace hod::core
{
	class ReflectionDescriptor;
}

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API ComponentFactory
	{
		_Singleton(ComponentFactory)

	public:

		template<typename _Component_>
		bool				Register();

		template<typename _Component_>
		bool				Unregister();

		const std::map<MetaType, ReflectionDescriptor*>& GetAllDescriptors() const { return _metaTypeToDescriptors; }

	protected:

		~ComponentFactory() = default;

	private:

		std::map<MetaType, ReflectionDescriptor*>	_metaTypeToDescriptors;
	};
}

#include "ComponentFactory.inl"
