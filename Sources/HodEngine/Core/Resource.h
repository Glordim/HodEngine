#pragma once

#include "HodEngine/Core/Reflection/ReflectionMacros.h"

namespace hod
{
	class Resource
	{
		REFLECTED_CLASS_NO_PARENT(Resource)

	public:

					Resource() = default;
					Resource(const Resource&) = delete;
					Resource(Resource&&) = delete;
		virtual		~Resource() = default;

		Resource&	operator = (const Resource&) = delete;
		Resource&	operator = (Resource&&) = delete;
	};
}
