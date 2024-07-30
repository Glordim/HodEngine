#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Document/DocumentReader.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Document/Document.hpp"

#include <fstream>

namespace hod
{
	/// @brief 
	/// @param document 
	/// @param path 
	/// @return 
	bool DocumentReader::Read(Document& document, const std::filesystem::path& path)
	{
		std::ifstream fileStream(path);
		return Read(document, fileStream);
	}

	bool DocumentReader::Read(Document& document, std::istream& stream, uint32_t size)
	{
		if (stream.fail())
		{
			OUTPUT_ERROR("Can't read document");
			return false;
		}

		if (document.GetRootNode().GetFirstChild() != nullptr)
		{
			OUTPUT_ERROR("Document not empty");
			return false;
		}

		return PopulateDocument(document, stream, size);
	}
}
