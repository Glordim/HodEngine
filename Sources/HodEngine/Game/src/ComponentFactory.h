#pragma once

#include <HodEngine/Core/Singleton.h>
#include "HodEngine/Core/Type.h"
#include "HodEngine/Game/src/Component.h"

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

			const std::map<MetaType, core::ReflectionDescriptor*>& GetAllDescriptors() const { return _metaTypeToDescriptors; }

		protected:

								ComponentFactory() = default;
								~ComponentFactory() override = default;

		private:

			std::map<MetaType, core::ReflectionDescriptor*>	_metaTypeToDescriptors;
		};
	}
}

#include "ComponentFactory.inl"
