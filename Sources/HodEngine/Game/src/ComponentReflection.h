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
		class ComponentReflection : public Singleton<ComponentReflection>
		{
			friend class Singleton<ComponentReflection>;

		public:

			template<typename __type__>
			bool				Register();

		protected:

								ComponentReflection() = default;
								~ComponentReflection() override = default;
		};
	}
}

#include "ComponentReflection.inl"
