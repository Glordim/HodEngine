#pragma once

#include <Core/Src/ReflectionTrait.h>

namespace HOD
{
	namespace GAME
	{
		namespace Traits
		{
			class Type : public CORE::ReflectionTrait
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

			public:

				void		Serialize(rapidjson::Document& document) const override;

			private:

				TypeEnum	_type;
			};
		}
	}
}