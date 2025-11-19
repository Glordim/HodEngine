#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cstdint>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <ostream>

namespace hod
{
	class Stream;
	class Document;

	/// @brief
	class HOD_CORE_API DocumentWriter
	{
	public:
		DocumentWriter() = default;
		DocumentWriter(const DocumentWriter&) = delete;
		DocumentWriter(DocumentWriter&&) = delete;
		virtual ~DocumentWriter() = default;

		DocumentWriter& operator=(const DocumentWriter&) = delete;
		DocumentWriter& operator=(DocumentWriter&&) = delete;

	public:
		bool Write(Document& document, const Path& path);
		bool Write(Document& document, std::ostream& stream);

	protected:
		virtual bool WriteDocument(Document& document, std::ostream& stream) = 0;
	};
}
