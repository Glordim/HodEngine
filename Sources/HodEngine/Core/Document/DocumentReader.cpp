#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/DocumentReader.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Document/Document.hpp"

namespace hod
{
	/// @brief 
	/// @param document 
	/// @param path 
	/// @return 
	bool DocumentReader::Read(Document& document, const std::filesystem::path& path)
	{
		FileSystem::Handle fileHandle = FileSystem::GetInstance()->Open(path);
		bool result = Read(document, fileHandle);
		FileSystem::GetInstance()->Close(fileHandle);
		return result;
	}

	/// @brief 
	/// @param document 
	/// @param fileHandle 
	/// @param size 
	/// @return 
	bool DocumentReader::Read(Document& document, FileSystem::Handle& fileHandle, uint32_t size)
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

		if (size == 0)
		{
			size = FileSystem::GetInstance()->GetSize(fileHandle);
		}

		char* buffer = new char[size + 1];
		if (FileSystem::GetInstance()->Read(fileHandle, buffer, size) != size)
		{
			return false;
		}
		buffer[size] = '\0';

		bool result = Read(document, buffer, size);
		delete[] buffer;
		return result;
	}

	/// @brief 
	/// @param buffer 
	/// @param size 
	/// @return 
	bool DocumentReader::Read(Document& document, const char* buffer, uint32_t size)
	{
		return PopulateDocument(document, buffer);
	}
}
