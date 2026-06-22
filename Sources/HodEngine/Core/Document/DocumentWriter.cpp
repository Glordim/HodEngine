#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/DocumentWriter.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"
#include "HodEngine/Core/Stream/MemoryStream.hpp"
#include "HodEngine/Core/String.hpp"

namespace hod::inline core
{
	bool DocumentWriter::Write(Document& document, const Path& path)
	{
		FileStream fileStream;
		if (fileStream.Open(path, FileSystem::OpenMode::Write) == false)
		{
			OUTPUT_ERROR("Can't write document at {}", path.GetString().CStr());
			return false;
		}
		return Write(document, fileStream);
	}

	bool DocumentWriter::Write(Document& document, Stream& stream)
	{
		return WriteDocument(document, stream);
	}

	bool DocumentWriter::Write(Document& document, String& output)
	{
		MemoryStream memoryStream;
		if (WriteDocument(document, memoryStream) == false)
		{
			return false;
		}
		output = String(static_cast<const char*>(memoryStream.GetBuffer()), memoryStream.GetSize());
		return true;
	}
}
