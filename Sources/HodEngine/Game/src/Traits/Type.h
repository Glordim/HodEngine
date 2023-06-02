#pragma once

#include <HodEngine/Core/Reflection/ReflectionTrait.h>

namespace hod
{
	namespace game
	{
		namespace Traits
		{
			class Type : public core::ReflectionTrait
			{
			public:

				enum TypeEnum
				{
					Component = 0,
					Actor,
					Count
				};
				inline static const char* _typeLabels[TypeEnum::Count] = { "Component", "Actor" };

			public:

									Type(TypeEnum type);

			private:

				TypeEnum			_type;
			};
		}
	}
}