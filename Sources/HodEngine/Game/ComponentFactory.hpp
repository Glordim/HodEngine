#pragma once

#include <HodEngine/Core/Singleton.hpp>
#include "HodEngine/Core/Type.hpp"
#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"

#include <map>

namespace hod
{
	namespace core
	{
		class ReflectionDescriptor;
	}

	namespace game
	{
		/// @brief 
		class ComponentFactory : public Singleton<ComponentFactory>
		{
			friend class Singleton<ComponentFactory>;

		public:

			template<typename _Component_>
			bool				Register();

			const std::map<MetaType, ReflectionDescriptor*>& GetAllDescriptors() const { return _metaTypeToDescriptors; }

		protected:

								ComponentFactory() = default;
								~ComponentFactory() override = default;

		private:

			std::map<MetaType, ReflectionDescriptor*>	_metaTypeToDescriptors;
		};
	}
}

#include "ComponentFactory.inl"
