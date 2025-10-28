#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/DocumentReader.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod
{
	/// @brief
	/// @param document
	/// @param path
	/// @return
	bool DocumentReader::Read(Document& document, const Path& path)
	{
		FileSystem::Handle fileHandle = FileSystem::GetInstance()->Open(path);
		bool               result = Read(document, fileHandle);
		FileSystem::GetInstance()->Close(fileHandle);
		return result;
	}

	/// @brief
	/// @param document
	/// @param fileHandle
	/// @param Size
	/// @return
	bool DocumentReader::Read(Document& document, FileSystem::Handle& fileHandle, uint32_t Size)
	{
		if (fileHandle.IsOpen() == false)
		{
			OUTPUT_ERROR("Can't read document");
			return false;
		}

		if (document.GetRootNode().GetFirstChild() != nullptr)
		{
			OUTPUT_ERROR("Document not empty");
			return false;
		}

		if (Size == 0)
		{
			Size = FileSystem::GetInstance()->GetSize(fileHandle);
		}

		char* buffer = DefaultAllocator::GetInstance().Allocate<char>(Size + 1);
		if (FileSystem::GetInstance()->Read(fileHandle, buffer, Size) != (int32_t)Size)
		{
			return false;
		}
		buffer[Size] = '\0';

		bool result = Read(document, buffer, Size);
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
