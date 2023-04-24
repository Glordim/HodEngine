#pragma once

#include <stdint.h>
#include <filesystem>

namespace hod
{
	namespace core
	{
		class Stream;
		class Document;

		/// @brief 
		class DocumentReader
		{
		public:

								DocumentReader() = default;
								DocumentReader(const DocumentReader&) = delete;
								DocumentReader(DocumentReader&&) = delete;
			virtual				~DocumentReader() = default;

			DocumentReader&		operator = (const DocumentReader&) = delete;
			DocumentReader&		operator = (DocumentReader&&) = delete;

		public:

			bool				Read(Document& document, const std::filesystem::path& path);
			bool				Read(Document& document, Stream& stream, uint32_t size = 0);

		protected:

			virtual bool		PopulateDocument(Document& document, Stream& stream, uint32_t size) = 0;
		};
	}
}
