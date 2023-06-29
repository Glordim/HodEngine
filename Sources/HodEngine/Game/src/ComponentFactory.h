#pragma once

#include <HodEngine/Core/Singleton.h>
#include "HodEngine/Core/Type.h"
#include "HodEngine/Game/src/Component.h"

#include <map>

namespace hod
{
	namespace game
	{
		class ComponentDescriptor;

		/// @brief 
		class ComponentFactory : public Singleton<ComponentFactory>
		{
			friend class Singleton<ComponentFactory>;

		public:

			template<typename _Component_>
			bool				Register();

			const std::map<MetaType, ComponentDescriptor*>& GetAllDescriptors() const { return _metaTypeToDescriptors; }

		protected:

								ComponentFactory() = default;
								~ComponentFactory() override = default;

		private:

			std::map<MetaType, ComponentDescriptor*>	_metaTypeToDescriptors;
		};
	}
}

#include "ComponentFactory.inl"
