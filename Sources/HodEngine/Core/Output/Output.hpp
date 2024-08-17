#pragma once
#include "HodEngine/Core/Export.hpp"

#include <HodEngine/Core/Type.hpp> // std::to_underlying

#include <string>

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

		Output(Type type, const std::string& content);
		Output(const Output& other) = default;
		Output(Output&& other) = default;
		~Output() = default;

		Output& operator=(const Output& other) = default;
		Output& operator=(Output&& other) = default;

	public:

		Type GetType() const;
		const std::string& GetContent() const;

	public:

		static const char* GetTypeName(Type type);
		
	private:

        Type _type;
        std::string _content;

	private:

		static const char* _typeNames[std::to_underlying(Type::Count)];
    };
}
