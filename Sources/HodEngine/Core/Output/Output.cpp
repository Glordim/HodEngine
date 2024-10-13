#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Output/Output.hpp"

namespace hod
{
	/// @brief 
	const char* Output::_typeNames[std::to_underlying(Type::Count)] = {
		"Message",
		"Warning",
		"Error",
		"Assert"
	};

	/// @brief 
	/// @param type 
	/// @param content 
	Output::Output(Type type, const std::string& content)
	: _type(type)
	, _content(content)
	{

	}

	/// @brief 
	/// @param type 
	/// @return 
	const char* Output::GetTypeName(Type type)
	{
		return _typeNames[std::to_underlying(type)];
	}

	/// @brief 
	/// @return 
	Output::Type Output::GetType() const
	{
		return _type;
	}

	/// @brief 
	/// @return 
	const std::string& Output::GetContent() const
	{
		return _content;
	}
}
