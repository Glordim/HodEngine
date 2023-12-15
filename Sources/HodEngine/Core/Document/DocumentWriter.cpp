#include "HodEngine/Core/Document/DocumentWriter.hpp"

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
