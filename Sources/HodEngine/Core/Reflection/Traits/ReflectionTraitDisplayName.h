#pragma once

#include "HodEngine/Core/Reflection/ReflectionTrait.h"

namespace hod
{
	namespace core
	{
		///@brief 
		class ReflectionTraitDisplayName : public ReflectionTrait
		{
			META_TYPE(ReflectionTraitDisplayName)

		public:

											ReflectionTraitDisplayName(const std::string_view& name);
											ReflectionTraitDisplayName(const ReflectionTraitDisplayName& copy) = default;
											ReflectionTraitDisplayName(ReflectionTraitDisplayName&& move) = default;
											~ReflectionTraitDisplayName() = default;

			ReflectionTraitDisplayName&		operator = (const ReflectionTraitDisplayName& copy) = default;
			ReflectionTraitDisplayName&		operator = (ReflectionTraitDisplayName&& move) = default;

		public:

			const std::string&				GetValue() const;

		private:

			std::string						_name;
		};
	}
}
