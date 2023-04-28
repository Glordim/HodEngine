#pragma once

#include "Core/Document/DocumentWriter.h"

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
		};
	}
}
