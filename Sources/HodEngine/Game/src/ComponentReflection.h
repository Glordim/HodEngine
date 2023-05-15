#pragma once

#include <HodEngine/Core/Singleton.h>

#include <vector>
#include <functional>

#include <rapidjson/document.h>

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

			bool				Dump(rapidjson::Value& rootNode, rapidjson::Document::AllocatorType& allocator);

		protected:

								ComponentReflection() = default;
								~ComponentReflection() override = default;

		private:

			std::vector<std::function<rapidjson::Value(rapidjson::Document::AllocatorType&)>>	_dumpFunctions;
		};
	}
}

#include "ComponentReflection.inl"
