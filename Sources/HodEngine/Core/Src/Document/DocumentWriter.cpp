#include "DocumentWriter.h"

#include "HodEngine/Core/Src/Output.h"
#include "HodEngine/Core/Src/Stream/Stream.h"
#include "HodEngine/Core/Src/Stream/FileStream.h"
#include "HodEngine/Core/Src/Document/Document.h"

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

			if (document.GetRootElement().GetFirstChild() == nullptr)
			{
				OUTPUT_ERROR("Document is empty");
				return false;
			}

			return WriteDocument(document, stream);
		}
	}
}
