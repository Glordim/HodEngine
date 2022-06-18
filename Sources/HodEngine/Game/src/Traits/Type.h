#pragma once

#include <HodEngine/Core/Src/Reflection/ReflectionTrait.h>

namespace hod
{
	namespace game
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

				rapidjson::Value	Serialize(rapidjson::Document::AllocatorType& allocator) const override;

			private:

				TypeEnum			_type;
			};
		}
	}
}