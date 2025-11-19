#pragma once
#include "HodEngine/Core/Export.hpp"

#include <HodEngine/Core/Type.hpp> // std::to_underlying

#include "HodEngine/Core/String.hpp"

namespace hod
{
	/// @brief
	struct HOD_CORE_API Output
	{
		enum class Type
		{
			Message = 0,
			Warning,
			Error,
			Assert,

			Count
		};

	public:
		Output(Type type, const String& content);
		Output(const Output& other) = default;
		Output(Output&& other) = default;
		~Output() = default;

		Output& operator=(const Output& other) = default;
		Output& operator=(Output&& other) = default;

	public:
		Type          GetType() const;
		const String& GetContent() const;

	public:
		static const char* GetTypeName(Type type);

	private:
		Type   _type;
		String _content;

	private:
		static const char* _typeNames[std::to_underlying(Type::Count)];
	};
}
