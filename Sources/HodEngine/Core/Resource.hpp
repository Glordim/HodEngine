#pragma once

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Core/UID.hpp"
#include <istream>

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

	public:

		const UID&	GetUid() const { return _uid; }

		virtual bool		Initialize(const Document::Node& documentNode, std::istream& stream) = 0;
		virtual void		Destroy() = 0;

	private:

		UID			_uid;
	};
}
