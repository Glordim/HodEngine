#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include <charconv>
#include <cstring>

namespace hod
{
	/// @brief
	void DocumentReaderJson::SkipWhiteSpace()
	{
		_cursor += std::strspn(_cursor, " \t\n\r");
	}

	/// @brief
	/// @param document
	/// @param stream
	/// @param size
	/// @return
	bool DocumentReaderJson::PopulateDocument(Document& document, const char* buffer)
	{
		_cursor = buffer;
		SkipWhiteSpace();
		bool parsingResult = ParseObject(document.GetRootNode());
		_cursor = nullptr;

		return parsingResult;
	}

	/// @brief
	/// @param json
	/// @param node
	/// @return
	bool DocumentReaderJson::ParseObject(Document::Node& node)
	{
		if (*_cursor != '{')
		{
			OUTPUT_ERROR("Json syntax error");
			return false;
		}
		++_cursor;
		SkipWhiteSpace();

		while (*_cursor != '}')
		{
			Document::Node* child = ParseKeyAndCreateChildNode(node);
			if (child == nullptr)
			{
				return false;
			}

			SkipWhiteSpace();

			if (*_cursor != ':')
			{
				OUTPUT_ERROR("Json syntax error");
				return false;
			}
			++_cursor;

			SkipWhiteSpace();

			if (ParseValue(*child) == false)
			{
				return false;
			}

			SkipWhiteSpace();

			if (*_cursor == '}')
			{
				break;
			}

			if (*_cursor == ',')
			{
				++_cursor;
				SkipWhiteSpace();
			}
			else
			{
				OUTPUT_ERROR("DocumentReaderJson::ParseObject Syntax error");
				return false;
			}
		}

		++_cursor;

		return true;
	}

	/// @brief
	/// @param json
	/// @param node
	/// @return
	bool DocumentReaderJson::ParseArray(Document::Node& node)
	{
		if (*_cursor != '[')
		{
			OUTPUT_ERROR("Json syntax error");
			return false;
		}
		++_cursor;
		SkipWhiteSpace();

		while (*_cursor != ']')
		{
			Document::Node& child = node.AddChild("");

			if (ParseValue(child) == false)
			{
				return false;
			}

			SkipWhiteSpace();

			if (*_cursor == ']')
			{
				break;
			}

			if (*_cursor == ',')
			{
				++_cursor;
				SkipWhiteSpace();
			}
			else
			{
				OUTPUT_ERROR("DocumentReaderJson::ParseArray Syntax error");
				return false;
			}
		}

		++_cursor;

		return true;
	}

	/// @brief
	/// @param node
	/// @return
	bool DocumentReaderJson::ParseValue(Document::Node& node)
	{
		if (*_cursor == '{') // object
		{
			return ParseObject(node);
		}
		else if (*_cursor == '[') // array
		{
			return ParseArray(node);
		}
		else if ((*_cursor >= '0' && *_cursor <= '9') || *_cursor == '-') // Number
		{
			bool isNegative = false;
			if (*_cursor == '-')
			{
				isNegative = true;

				if (*(_cursor + 1) < '0' || *(_cursor + 1) > '9')
				{
					OUTPUT_ERROR("Json syntax error");
					return false;
				}
			}

			bool        isFloat = false;
			const char* valueStart = _cursor;
			_cursor += std::strspn(valueStart, "-0123456789");
			if (*_cursor == '.' || *_cursor == 'e' || *_cursor == 'E')
			{
				++_cursor;
				isFloat = true;
				_cursor += std::strspn(_cursor, "0123456789e-+");
			}
			const char* valueEnd = _cursor;
			if (isFloat == true)
			{
				double value;
				/* std::from_chars with Floating is not available for now on Clang
				std::from_chars_result result = std::from_chars(valueStart, valueEnd, value);
				if (result.ec != std::errc())
				{
				    OUTPUT_ERROR("Json syntax error");
				    return false;
				}
				*/
				char* end = (char*)valueEnd;
				value = std::strtod(valueStart, &end);
				node.SetFloat64(value);
				return true;
			}
			else
			{
				if (isNegative == false)
				{
					uint64_t               value;
					std::from_chars_result result = std::from_chars(valueStart, valueEnd, value);
					if (result.ec != std::errc())
					{
						OUTPUT_ERROR("Json syntax error");
						return false;
					}
					node.SetUInt64(value);
					return true;
				}
				else
				{
					int64_t                value;
					std::from_chars_result result = std::from_chars(valueStart, valueEnd, value);
					if (result.ec != std::errc())
					{
						OUTPUT_ERROR("Json syntax error");
						return false;
					}
					node.SetInt64(value);
					return true;
				}
			}
		}
		else if (*_cursor == '\"') // string
		{
			++_cursor;
			const char* valueStart = _cursor;
			const char* valueEnd = std::strstr(valueStart, "\"");
			while (valueEnd != nullptr && valueEnd[-1] == '\\')
			{
				valueEnd = std::strstr(valueEnd + 1, "\"");
			}
			_cursor = valueEnd;
			++_cursor;

			String value(valueStart, valueEnd - valueStart);

			std::size_t offset = 0;
			std::size_t index = value.Find("\\t", offset);
			while (index != String::Npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.Replace(index, 2, "\t");
				}
				index = value.Find("\\t", offset);
			}

			offset = 0;
			index = value.Find("\\n", offset);
			while (index != String::Npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.Replace(index, 2, "\n");
				}
				index = value.Find("\\n", offset);
			}

			offset = 0;
			index = value.Find("\\r", offset);
			while (index != String::Npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.Replace(index, 2, "\r");
				}
				index = value.Find("\\r", offset);
			}

			offset = 0;
			index = value.Find("\\f", offset);
			while (index != String::Npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.Replace(index, 2, "\f");
				}
				index = value.Find("\\f", offset);
			}

			offset = 0;
			index = value.Find("\\b", offset);
			while (index != String::Npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.Replace(index, 2, "\b");
				}
				index = value.Find("\\b", offset);
			}

			offset = 0;
			index = value.Find("\\\"", offset);
			while (index != String::Npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.Replace(index, 2, "\"");
				}
				index = value.Find("\\\"", offset);
			}

			offset = 0;
			index = value.Find("\\\\", offset);
			while (index != String::Npos)
			{
				offset = index + 1;
				value.Replace(index, 2, "\\");
				index = value.Find("\\\\", offset);
			}

			node.SetString(std::string_view(value.CStr(), value.Size()));

			return true;
		}
		else if (std::strncmp(_cursor, "null", sizeof("null") - 1) == 0) // null
		{
			_cursor += sizeof("null") - 1;
			return false;
		}
		else if (std::strncmp(_cursor, "true", sizeof("true") - 1) == 0) // true (bool)
		{
			_cursor += sizeof("true") - 1;
			node.SetBool(true);
			return true;
		}
		else if (std::strncmp(_cursor, "false", sizeof("false") - 1) == 0) // false (bool)
		{
			_cursor += sizeof("false") - 1;
			node.SetBool(false);
			return true;
		}

		OUTPUT_ERROR("Json syntax error");
		return false;
	}

	/// @brief
	/// @param node
	/// @return
	Document::Node* DocumentReaderJson::ParseKeyAndCreateChildNode(Document::Node& node)
	{
		if (*_cursor != '\"')
		{
			OUTPUT_ERROR("Json syntax error");
			return nullptr;
		}
		++_cursor;

		const char* keyStart = _cursor;
		_cursor = std::strchr(keyStart, '"');
		if (_cursor == nullptr)
		{
			OUTPUT_ERROR("Json syntax error");
			return nullptr;
		}
		const char* keyEnd = _cursor;
		++_cursor;

		Document::Node& child = node.AddChild(std::string_view(keyStart, keyEnd - keyStart));
		return &child;
	}
}
