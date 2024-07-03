#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Core/Document/DocumentReader.hpp"
#include "HodEngine/Core/Document/Document.hpp"

namespace hod
{
	/// @brief 
	class HOD_API DocumentReaderJson : public DocumentReader
	{
	public:

								DocumentReaderJson() = default;
								DocumentReaderJson(const DocumentReaderJson&) = delete;
								DocumentReaderJson(DocumentReaderJson&&) = delete;
								~DocumentReaderJson() override = default;

		DocumentReaderJson&		operator = (const DocumentReaderJson&) = delete;
		DocumentReaderJson&		operator = (DocumentReaderJson&&) = delete;

	protected:

		bool					PopulateDocument(Document& document, std::istream& stream, uint32_t size) override;

	private:

		bool					ParseNode(Document::Node& node);
		bool					ParseObject(Document::Node& node);
		bool					ParseArray(Document::Node& node);
		Document::Node*			ParseKeyAndCreateChildNode(Document::Node& node);
		bool					ParseValue(Document::Node& node);

		void					SkipWhiteSpace();

	private:

		const char*				_cursor = nullptr;
	};
}
