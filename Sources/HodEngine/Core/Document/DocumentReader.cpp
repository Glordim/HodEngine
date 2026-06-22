#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/DocumentReader.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"

namespace hod::inline core
{
	bool DocumentReader::Read(Document& document, const Path& path)
	{
		FileStream fileStream;
		if (fileStream.Open(path) == false)
		{
			OUTPUT_ERROR("Can't read document at {}", path.GetString().CStr());
			return false;
		}
		return Read(document, fileStream);
	}

	bool DocumentReader::Read(Document& document, Stream& stream, uint32_t size)
	{
		if (document.GetRootNode().GetFirstChild() != nullptr)
		{
			OUTPUT_ERROR("Document not empty");
			return false;
		}

		if (size == 0)
		{
			size = stream.GetSize();
		}

		char* buffer = DefaultAllocator::GetInstance().Allocate<char>(size + 1);
		if (stream.Read(buffer, size) != size)
		{
			DefaultAllocator::GetInstance().Free(buffer);
			return false;
		}
		buffer[size] = '\0';

		bool result = Read(document, buffer, size);
		DefaultAllocator::GetInstance().Free(buffer);
		return result;
	}

	/// @brief
	/// @param buffer
	/// @param Size
	/// @return
	bool DocumentReader::Read(Document& document, const char* buffer, uint32_t Size)
	{
		(void)Size; // TODO
		return PopulateDocument(document, buffer);
	}
}
