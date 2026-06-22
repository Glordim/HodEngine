#pragma once
#include "HodEngine/Core/Export.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>

namespace hod::inline core
{
	class Stream;
	class String;
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
		bool Write(Document& document, Stream& stream);
		bool Write(Document& document, String& output);

	protected:
		virtual bool WriteDocument(Document& document, Stream& stream) = 0;
	};
}
