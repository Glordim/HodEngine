#include "Core/Document/DocumentWriter.h"

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
		bool DocumentWriter::Write(Document& document, const std::filesystem::path& path)
		{
			FileStream fileStream(path, FileMode::Write);
			return Write(document, fileStream);
		}

		/// @brief 
		/// @param document 
		/// @param stream 
		/// @param size 
		/// @return 
		bool DocumentWriter::Write(Document& document, Stream& stream)
		{
			if (stream.CanWrite() == false)
			{
				OUTPUT_ERROR("Can't write document");
				return false;
			}

			if (document.GetRootNode().GetFirstChild() == nullptr)
			{
				OUTPUT_ERROR("Document is empty");
				return false;
			}

			return WriteDocument(document, stream);
		}
	}
}
