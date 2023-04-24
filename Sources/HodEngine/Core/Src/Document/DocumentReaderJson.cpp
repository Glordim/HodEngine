#include "DocumentReaderJson.h"

#include "HodEngine/Core/Src/Stream/Stream.h"

#include "HodEngine/Core/Src/Output.h"

namespace hod
{
	namespace core
	{
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

			const char* json = SkipWhiteSpace(buffer);
			if (*json != '{')
			{
				OUTPUT_ERROR("Json syntax error");
				delete[] buffer;
				return nullptr;
			}
			const char* parsingResult = ParseNode(json + 1, document.GetRootElement());

			delete[] buffer;

			return (parsingResult != nullptr);
		}

		const char* SkipWhiteSpace(const char* str)
		{
			return str + std::strspn(str, " \t\n\r");
		}

		/// @brief 
		/// @param json 
		/// @param element 
		/// @return 
		const char* DocumentReaderJson::ParseNode(const char* json, Document::Element& element)
		{
			json = SkipWhiteSpace(json);

			while (*json == '\"')
			{
				const char* keyStart = json + 1;
				json = json + std::strspn(keyStart, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-");
				if (*json != '\"')
				{
					OUTPUT_ERROR("Json syntax error");
					return nullptr;
				}
				const char* keyEnd = json - 1;
				json = SkipWhiteSpace(json + 1);

				Document::Element& child = element.AddChild(keyStart, keyEnd - keyStart);

				if (*json != ':')
				{
					OUTPUT_ERROR("Json syntax error");
					return nullptr;
				}

				json = SkipWhiteSpace(json + 1);

				if (*json >= '0' || *json <= '9') // Number
				{

				}
				else if (*json == '\"') // string
				{
					const char* valueEnd = std::strstr(json + 1, "\"");
					while (valueEnd != nullptr && valueEnd[-1] == '\\')
					{
						valueEnd = std::strstr(valueEnd + 1, "\"");
					}

					child.SetValue(std::move(std::string(valueStart, valueEnd)));
				}
				else if (std::strncmp(json, "null", sizeof("null")) == 0) // null
				{
				}
				else if (std::strncmp(json, "true", sizeof("true")) == 0) // true (bool)
				{
				}
				else if (std::strncmp(json, "false", sizeof("false")) == 0)
				{
				}
				else if (*json == '[') // array
				{

				}
				else if (*json == '{') // object
				{

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
	}
}
