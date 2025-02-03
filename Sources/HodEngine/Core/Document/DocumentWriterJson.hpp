#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Document/DocumentWriter.hpp"
#include "HodEngine/Core/Document/Document.hpp"

namespace hod
{
	/// @brief 
	class HOD_CORE_API DocumentWriterJson : public DocumentWriter
	{
	public:

							DocumentWriterJson() = default;
							DocumentWriterJson(const DocumentWriterJson&) = delete;
							DocumentWriterJson(DocumentWriterJson&&) = delete;
							~DocumentWriterJson() override = default;

		DocumentWriterJson&	operator = (const DocumentWriterJson&) = delete;
		DocumentWriterJson&	operator = (DocumentWriterJson&&) = delete;

	protected:

		bool				WriteDocument(Document& document, std::ostream& stream) override;

	private:

		bool				WriteNode(const Document::Node& node, std::ostream& stream);

	private:

		bool				_pretty = true;
		uint32_t			_indentation = 0;
	};
}
