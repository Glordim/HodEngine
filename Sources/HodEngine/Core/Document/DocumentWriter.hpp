#pragma once
#include <HodEngine/HodEngine.hpp>

#include <filesystem>
#include <cstdint>
#include <ostream>

namespace hod
{
	class Stream;
	class Document;

	/// @brief 
	class HOD_API DocumentWriter
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
		bool				Write(Document& document, std::ostream& stream);

	protected:

		virtual bool		WriteDocument(Document& document, std::ostream& stream) = 0;
	};
}
