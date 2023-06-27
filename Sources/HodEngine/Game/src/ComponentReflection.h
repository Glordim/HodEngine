#pragma once

#include <HodEngine/Core/Singleton.h>

#include <vector>
#include <functional>

namespace hod
{
	namespace game
	{
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class ComponentFactory : public Singleton<ComponentFactory>
		{
			friend class Singleton<ComponentFactory>;

		public:

			template<typename __type__>
			bool				Register();

		protected:

								ComponentFactory() = default;
								~ComponentFactory() override = default;
		};
	}
}

#include "ComponentReflection.inl"
