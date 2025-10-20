#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Core/UID.hpp"

namespace hod
{
	class HOD_CORE_API Resource : public std::enable_shared_from_this<Resource>
	{
		REFLECTED_CLASS_NO_PARENT(Resource)

	public:
		struct Data
		{
			void*    _buffer = nullptr;
			uint32_t _size = 0;
		};

	public:
		Resource() = default;
		Resource(const Resource&) = delete;
		Resource(Resource&&) = delete;
		virtual ~Resource() = default;

		Resource& operator=(const Resource&) = delete;
		Resource& operator=(Resource&&) = delete;

	public:
		const UID& GetUid() const
		{
			return _uid;
		}

		void SetUid(const UID& uid)
		{
			_uid = uid;
		}

		virtual bool Initialize(const Document::Node& documentNode, const Vector<Resource::Data>& datas) = 0;

	private:
		UID _uid;
	};
}
