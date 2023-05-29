#pragma once

#include "HodEngine/Core/Reflection/ReflectionMacros.h"

namespace hod::core
{
	class Resource
	{
		REFLECTED_ABSTRACT_CLASS(Resource)

	public:

					Resource() = default;
					Resource(const Resource&) = delete;
					Resource(Resource&&) = delete;
		virtual		~Resource() = default;

		Resource&	operator = (const Resource&) = delete;
		Resource&	operator = (Resource&&) = delete;

	public:
	
	};
}