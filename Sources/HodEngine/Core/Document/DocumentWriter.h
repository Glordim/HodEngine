#pragma once

#include <filesystem>
#include <cstdint>

namespace hod
{
	class Stream;
	class Document;

	/// @brief 
	class DocumentWriter
	{
	public:

							DocumentWriter() = default;
							DocumentWriter(const DocumentWriter&) = delete;
							DocumentWriter(DocumentWriter&&) = delete;
		virtual				~DocumentWriter() = default;

		DocumentWriter&		operator = (const DocumentWriter&) = delete;
		DocumentWriter&		operator = (DocumentWriter&&) = delete;

	public:

		bool				Write(Document& document, const std::filesystem::path& path);
		bool				Write(Document& document, Stream& stream);

	protected:

		virtual bool		WriteDocument(Document& document, Stream& stream) = 0;
	};
}
