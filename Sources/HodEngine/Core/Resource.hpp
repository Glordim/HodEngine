#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/UID.hpp"
#include <istream>

namespace hod
{
	class HOD_CORE_API Resource
	{
		REFLECTED_CLASS_NO_PARENT(Resource, HOD_CORE_API)

	public:

					Resource() = default;
					Resource(const Resource&) = delete;
					Resource(Resource&&) = delete;
		virtual		~Resource() = default;

		Resource&	operator = (const Resource&) = delete;
		Resource&	operator = (Resource&&) = delete;

		bool		operator==(const Resource& other) const = default;

	public:

		const UID&	GetUid() const { return _uid; }
		void		SetUid(const UID& uid) { _uid = uid; }

		virtual bool		Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle) = 0;

	private:

		UID			_uid;
	};
}
