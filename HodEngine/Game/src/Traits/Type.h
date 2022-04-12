#pragma once

#include <Core/Src/ReflectionTrait.h>

namespace HOD
{
	namespace GAME
	{
		namespace Traits
		{
			class Type : public CORE::RefectionTrait
			{
			public:

				enum TypeEnum
				{
					Component = 0,
					Actor,
					Count
				};
				static const char* _typeLabels[TypeEnum::Count] = { "Component", "Actor" };

			public:

				Type(TypeEnum type);

			public:

				void		Serialize(rapidjson::Document& document) override;

			private:

				TypeEnum	_type;
			};
		}
	}
}