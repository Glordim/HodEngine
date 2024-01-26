#include "HodEngine/Core/Document/DocumentReaderJson.hpp"

#include "HodEngine/Core/Stream/Stream.hpp"

#include "HodEngine/Core/Output.hpp"
#include "HodEngine/Core/StringConversion.hpp"

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
	bool DocumentReaderJson::PopulateDocument(Document& document, Stream& stream, uint32_t size)
	{
		if (size == 0)
		{
			size = stream.GetSize(); // todo see istrem implementation
		}

		char* buffer = new char[size + 1];
		if (stream.Read((void*)buffer, size) == false)
		{
			return false;
		}
		buffer[size] = '\0';

		_cursor = buffer;
		SkipWhiteSpace();
		bool parsingResult = ParseObject(document.GetRootNode());

		delete[] buffer;
		_cursor = nullptr;

		return parsingResult;
	}

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @param size 
	/// @return 
	bool DocumentReaderJson::PopulateDocument(Document& document, std::istream& stream, uint32_t size)
	{
		if (size == 0)
		{
			std::streampos initialPos = stream.tellg();
			stream.seekg(0, std::ios::end);
			size = stream.tellg() - initialPos;
			stream.seekg(initialPos, std::ios::beg);
		}

		char* buffer = new char[size + 1];
		stream.read(buffer, size);
		if (stream.fail())
		{
			return false;
		}
		buffer[size] = '\0';

		_cursor = buffer;
		SkipWhiteSpace();
		bool parsingResult = ParseObject(document.GetRootNode());

		delete[] buffer;
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
				++_cursor;

				if (*_cursor < '0' || *_cursor > '9')
				{
					OUTPUT_ERROR("Json syntax error");
					return false;
				}
			}

			bool isFloat = false;
			const char* valueStart = _cursor;
			_cursor += std::strspn(valueStart, "0123456789");
			if (*_cursor == '.')
			{
				++_cursor;
				isFloat = true;
				_cursor += std::strspn(_cursor, "0123456789");
			}
			const char* valueEnd = _cursor;
			if (isFloat == true)
			{
				double value;
				if (StringConversion::StringToFloat64(std::string_view(valueStart, valueEnd - valueStart), value) == false)
				{
					OUTPUT_ERROR("Json syntax error");
					return false;
				}
				node.SetFloat64(value);
				return true;
			}
			else
			{
				if (isNegative == false)
				{
					uint64_t value;
					if (StringConversion::StringToUInt64(std::string_view(valueStart, valueEnd - valueStart), value) == false)
					{
						OUTPUT_ERROR("Json syntax error");
						return false;
					}
					node.SetUInt64(value);
					return true;
				}
				else
				{
					int64_t value;
					if (StringConversion::StringToInt64(std::string_view(valueStart, valueEnd - valueStart), value) == false)
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

			std::string value(valueStart, valueEnd - valueStart);

			std::size_t offset = 0;
			std::size_t index = value.find("\\t", offset);
			while (index != std::string::npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.replace(index, 2, "\t");
				}
				index = value.find("\\t", offset);
			}

			offset = 0;
			index = value.find("\\n", offset);
			while (index != std::string::npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.replace(index, 2, "\n");
				}
				index = value.find("\\n", offset);
			}

			offset = 0;
			index = value.find("\\r", offset);
			while (index != std::string::npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.replace(index, 2, "\r");
				}
				index = value.find("\\r", offset);
			}

			offset = 0;
			index = value.find("\\f", offset);
			while (index != std::string::npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.replace(index, 2, "\f");
				}
				index = value.find("\\f", offset);
			}

			offset = 0;
			index = value.find("\\b", offset);
			while (index != std::string::npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.replace(index, 2, "\b");
				}
				index = value.find("\\b", offset);
			}

			offset = 0;
			index = value.find("\\\"", offset);
			while (index != std::string::npos)
			{
				offset = index + 1;
				if (index > 0 && value[index - 1] != '\\')
				{
					value.replace(index, 2, "\"");
				}
				index = value.find("\\\"", offset);
			}

			offset = 0;
			index = value.find("\\\\", offset);
			while (index != std::string::npos)
			{
				offset = index + 1;
				value.replace(index, 2, "\\");
				index = value.find("\\\\", offset);
			}

			node.SetString(std::string_view(value.data(), value.size()));

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
		_cursor += std::strspn(keyStart, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-");
		if (*_cursor != '\"')
		{
			OUTPUT_ERROR("Json syntax error");
			return nullptr;
		}
		const char* keyEnd = _cursor;
		++_cursor;

		Document::Node& child = node.AddChild(std::string_view(keyStart, (keyEnd - keyStart)));
		return &child;
	}
}
