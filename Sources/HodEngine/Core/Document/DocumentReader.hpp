#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <stdint.h>
#include <filesystem>
#include <istream>

namespace hod
{
	class Stream;
	class Document;

	/// @brief 
	class HOD_CORE_API DocumentReader
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
		bool				Read(Document& document, FileSystem::Handle& fileHandle, uint32_t size = 0);

	protected:

		virtual bool		PopulateDocument(Document& document, const char* buffer) = 0;
	};
}
