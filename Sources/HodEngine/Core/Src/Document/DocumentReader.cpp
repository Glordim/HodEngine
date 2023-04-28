#include "Core/Document/DocumentReader.h"

#include "Core/Output.h"
#include "Core/Stream/Stream.h"
#include "Core/Stream/FileStream.h"
#include "Core/Document/Document.h"

namespace hod
{
	namespace core
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
}
