#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Document/DocumentNode.hpp"

#include <cstdint>
#include <map>
#include <string_view>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod::inline core
{
	class HOD_CORE_API Document
	{
		REFLECTED_CLASS_NO_VIRTUAL(Document)

		friend class DocumentNode;

	private:
		using StringId = uint64_t;

	public:
		Document() = default;
		Document(const Document&) = delete;
		Document(Document&&) = delete;
		~Document() = default;

		Document& operator=(const Document& right);
		Document& operator=(Document&&) = delete;

	public:
		DocumentNode& GetRootNode() const;

	private:
		StringId      AddString(const std::string_view& str);
		const String& GetString(StringId hash);

	private:
		DocumentNode _root{*this, ""};

		std::map<uint64_t, String> _stringTable;
	};
}
