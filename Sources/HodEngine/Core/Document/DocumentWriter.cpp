#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/DocumentWriter.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <fstream>

namespace hod
{
	/// @brief
	/// @param document
	/// @param path
	/// @return
	bool DocumentWriter::Write(Document& document, const Path& path)
	{
		std::ofstream fileStream(path.GetString().CStr());
		return Write(document, fileStream);
	}

	/// @brief
	/// @param document
	/// @param stream
	/// @param size
	/// @return
	bool DocumentWriter::Write(Document& document, std::ostream& stream)
	{
		if (stream.fail())
		{
			OUTPUT_ERROR("Can't write document");
			return false;
		}

		return WriteDocument(document, stream);
	}
}
