#pragma once

#include "HodEngine/Core/Document/DocumentWriter.h"
#include "HodEngine/Core/Document/Document.h"

namespace hod
{
	namespace core
	{
		/// @brief 
		class DocumentWriterJson : public DocumentWriter
		{
		public:

								DocumentWriterJson() = default;
								DocumentWriterJson(const DocumentWriterJson&) = delete;
								DocumentWriterJson(DocumentWriterJson&&) = delete;
								~DocumentWriterJson() override = default;

			DocumentWriterJson&	operator = (const DocumentWriterJson&) = delete;
			DocumentWriterJson&	operator = (DocumentWriterJson&&) = delete;

		protected:

			bool				WriteDocument(Document& document, Stream& stream) override;

		private:

			bool				WriteNode(const Document::Node& node, Stream& stream);
		};
	}
}