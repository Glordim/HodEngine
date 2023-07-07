#include "HodEngine/Core/Document/DocumentReader.h"

#include "HodEngine/Core/Output.h"
#include "HodEngine/Core/Stream/Stream.h"
#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/Document/Document.h"

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
