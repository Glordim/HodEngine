#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReader.hpp"

namespace hod::inline core
{
	/// @brief
	class HOD_CORE_API DocumentReaderJson : public DocumentReader
	{
	public:
		DocumentReaderJson() = default;
		DocumentReaderJson(const DocumentReaderJson&) = delete;
		DocumentReaderJson(DocumentReaderJson&&) = delete;
		~DocumentReaderJson() override = default;

		DocumentReaderJson& operator=(const DocumentReaderJson&) = delete;
		DocumentReaderJson& operator=(DocumentReaderJson&&) = delete;

	protected:
		bool PopulateDocument(Document& document, const char* buffer) override;

	private:
		bool            ParseNode(DocumentNode& node);
		bool            ParseObject(DocumentNode& node);
		bool            ParseArray(DocumentNode& node);
		DocumentNode* ParseKeyAndCreateChildNode(DocumentNode& node);
		bool            ParseValue(DocumentNode& node);

		void SkipWhiteSpace();

	private:
		const char* _cursor = nullptr;
	};
}
