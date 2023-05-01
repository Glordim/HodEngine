#pragma once

#include "HodEngine/Core/Document/DocumentReader.h"
#include "HodEngine/Core/Document/Document.h"

namespace hod
{
	namespace core
	{
		/// @brief 
		class DocumentReaderJson : public DocumentReader
		{
		public:

									DocumentReaderJson() = default;
									DocumentReaderJson(const DocumentReaderJson&) = delete;
									DocumentReaderJson(DocumentReaderJson&&) = delete;
									~DocumentReaderJson() override = default;

			DocumentReaderJson&		operator = (const DocumentReaderJson&) = delete;
			DocumentReaderJson&		operator = (DocumentReaderJson&&) = delete;

		protected:

			bool					PopulateDocument(Document& document, Stream& stream, uint32_t size) override;

		private:

			const char*				ParseNode(const char* json, Document::Node& node);
		};
	}
}
