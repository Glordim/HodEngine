#include "HodEngine/Core/Document/DocumentReaderJson.h"

#include "HodEngine/Core/Stream/Stream.h"

#include "HodEngine/Core/Output.h"
#include "HodEngine/Core/StringConversion.h"

namespace hod
{
	namespace core
	{
		const char* SkipWhiteSpace(const char* str)
		{
			return str + std::strspn(str, " \t\n\r");
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

			_cursor = SkipWhiteSpace(buffer);
			if (*_cursor != '{')
			{
				OUTPUT_ERROR("Json syntax error");
				delete[] buffer;
				return false;
			}
			++_cursor;
			bool parsingResult = ParseNode(document.GetRootNode());

			delete[] buffer;
			_cursor = nullptr;

			return parsingResult;
		}

		/// @brief 
		/// @param json 
		/// @param node 
		/// @return 
		bool DocumentReaderJson::ParseNode(Document::Node& node)
		{
			json = SkipWhiteSpace(json);

			while (*json == '\"')
			{
				const char* keyStart = json + 1;
				json = json + std::strspn(keyStart, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") + 1;
				if (*json != '\"')
				{
					OUTPUT_ERROR("Json syntax error");
					return nullptr;
				}
				const char* keyEnd = json;
				json = SkipWhiteSpace(json + 1);

				Document::Node& child = node.AddChild(std::string_view(keyStart, (keyEnd - keyStart)));

				if (*json != ':')
				{
					OUTPUT_ERROR("Json syntax error");
					return nullptr;
				}

				json = SkipWhiteSpace(json + 1);

				if ((*json >= '0' && *json <= '9') || *json == '-') // Number
				{
					bool isNegative = false;
					if (*json == '-')
					{
						isNegative = true;
						++json;
					}

					if (*json < '0' || *json > '9')
					{
						OUTPUT_ERROR("Json syntax error");
						return nullptr;
					}

					bool isFloat = false;
					const char* valueStart = json;
					json += std::strspn(valueStart, "0123456789");
					if (*json == '.')
					{
						isFloat = true;
						json += std::strspn(json, "0123456789");
					}
					const char* valueEnd = json;
					if (isFloat == true)
					{
						double value;
						if (StringConversion::StringToFloat64(std::string_view(valueStart, valueEnd - valueStart), value) == false)
						{
							OUTPUT_ERROR("Json syntax error");
							return nullptr;
						}
						child.SetFloat64(value);
					}
					else
					{
						if (isNegative == false)
						{
							uint64_t value;
							if (StringConversion::StringToUInt64(std::string_view(valueStart, valueEnd - valueStart), value) == false)
							{
								OUTPUT_ERROR("Json syntax error");
								return nullptr;
							}
							child.SetUInt64(value);
						}
						else
						{
							int64_t value;
							if (StringConversion::StringToInt64(std::string_view(valueStart, valueEnd - valueStart), value) == false)
							{
								OUTPUT_ERROR("Json syntax error");
								return nullptr;
							}
							child.SetInt64(value);
						}
					}
				}
				else if (*json == '\"') // string
				{
					const char* valueStart = json + 1;
					const char* valueEnd = std::strstr(valueStart, "\"");
					while (valueEnd != nullptr && valueEnd[-1] == '\\')
					{
						valueEnd = std::strstr(valueEnd + 1, "\"");
					}

					child.SetString(std::string_view(valueStart, valueEnd - valueStart));
				}
				else if (std::strncmp(json, "null", sizeof("null")) == 0) // null
				{
				}
				else if (std::strncmp(json, "true", sizeof("true")) == 0) // true (bool)
				{
					child.SetBool(true);
				}
				else if (std::strncmp(json, "false", sizeof("false")) == 0)
				{
					child.SetBool(false);
				}
				else if (*json == '[') // array
				{
					json = SkipWhiteSpace(json + 1);
					while (*json != ']')
					{
						Document::Node& arrayItem = child.AddChild("");
						json = ParseNode(json, arrayItem);
						json = SkipWhiteSpace(json + 1);
					}
				}
				else if (*json == '{') // object
				{
					json = ParseNode(json + 1, child);
					json = SkipWhiteSpace(json);
					if (*json == '}')
					{
						json = json + 1;
					}
					else
					{
						OUTPUT_ERROR("Json syntax error");
						return nullptr;
					}
				}
				else
				{
					OUTPUT_ERROR("Json syntax error");
					return nullptr;
				}

				json = SkipWhiteSpace(json + 1);

				if (*json == ',')
				{
					json = SkipWhiteSpace(json + 1);
					continue;
				}
				else if (*json == '}')
				{
					break;
				}
			}

			if (*json != '}')
			{
				OUTPUT_ERROR("Json syntax error");
				return nullptr;
			}

			return json + 1;
		}

		/// @brief 
		/// @param node 
		/// @return 
		Document::Node* DocumentReaderJson::ParseKeyAndCreateChildNode(Document::Node& node)
		{
			const char* keyStart = _cursor + 1;
			_cursor = _cursor + std::strspn(keyStart, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") + 1;
			if (*_cursor != '\"')
			{
				OUTPUT_ERROR("Json syntax error");
				return nullptr;
			}
			const char* keyEnd = _cursor;

			Document::Node& child = node.AddChild(std::string_view(keyStart, (keyEnd - keyStart)));
			return &child;
		}
	}
}
