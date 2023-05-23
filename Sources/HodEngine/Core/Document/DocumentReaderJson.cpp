#include "HodEngine/Core/Document/DocumentReaderJson.h"

#include "HodEngine/Core/Stream/Stream.h"

#include "HodEngine/Core/Output.h"
#include "HodEngine/Core/StringConversion.h"

namespace hod
{
	namespace core
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
				size = stream.GetSize();
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

				node.SetString(std::string_view(valueStart, valueEnd - valueStart));
				return true;
			}
			else if (std::strncmp(_cursor, "null", sizeof("null")) == 0) // null
			{
				return false;
			}
			else if (std::strncmp(_cursor, "true", sizeof("true")) == 0) // true (bool)
			{
				node.SetBool(true);
				return true;
			}
			else if (std::strncmp(_cursor, "false", sizeof("false")) == 0) // false (bool)
			{
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
}
