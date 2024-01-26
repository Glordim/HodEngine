#include "HodEngine/Core/Document/DocumentReader.hpp"

#include "HodEngine/Core/Output.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"
#include "HodEngine/Core/Document/Document.hpp"

namespace hod
{
	/// @brief 
	/// @param document 
	/// @param path 
	/// @return 
	bool DocumentReader::Read(Document& document, const std::filesystem::path& path)
	{
		FileStream fileStream(path, FileMode::Read);
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

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @param size 
	/// @return 
	bool DocumentReader::Read(Document& document, Stream& stream, uint32_t size)
	{
		if (stream.CanRead() == false)
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
