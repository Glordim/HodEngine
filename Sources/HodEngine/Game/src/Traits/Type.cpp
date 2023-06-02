#include "Type.h"

namespace hod
{
	namespace game
	{
		namespace Traits
		{
			Type::Type(TypeEnum type)
			: core::ReflectionTrait("Type")
			, _type(type)
			{

			}
		}
	}
}
