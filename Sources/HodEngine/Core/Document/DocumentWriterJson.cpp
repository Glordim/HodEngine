#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"

#include <algorithm>
#include <cstring>
#include <charconv>
#include <cassert>

namespace hod
{
	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool DocumentWriterJson::WriteDocument(Document& document, std::ostream& stream)
	{
		return WriteNode(document.GetRootNode(), stream);
	}

	/// @brief 
	/// @param node 
	/// @param stream 
	/// @return 
	bool DocumentWriterJson::WriteNode(const Document::Node& node, std::ostream& stream)
	{
		const std::string& name = node.GetName();
		if (name.size() > 0)
		{
			stream.write("\"", 1);
			stream.write(name.data(), name.size());
			stream.write("\":", 2);
		}

		switch (node.GetType())
		{
			case Document::Node::Type::Object:
			{
				stream.write("{", 1);
				Document::Node* child = node.GetFirstChild();
				while (child != nullptr)
				{
					WriteNode(*child, stream);
					child = child->GetNextSibling();
					if (child != nullptr)
					{
						stream.write(",", 1);
					}
				}
				stream.write("}", 1);
			}
			break;

			case Document::Node::Type::Array:
			{
				stream.write("[", 1);
				Document::Node* child = node.GetFirstChild();
				while (child != nullptr)
				{
					WriteNode(*child, stream);
					child = child->GetNextSibling();
					if (child != nullptr)
					{
						stream.write(",", 1);
					}
				}
				stream.write("]", 1);
			}
			break;

			case Document::Node::Type::Bool:
			{
				if (node.GetBool() == true)
				{
					stream.write("true", 4);
				}
				else
				{
					stream.write("false", 5);
				}
			}
			break;

			case Document::Node::Type::Int8:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetInt8());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Int16:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetInt16());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Int32:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetInt32());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Int64:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetInt64());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::UInt8:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetUInt8());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::UInt16:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetUInt16());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::UInt32:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetUInt32());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::UInt64:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetUInt64());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Float32:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetFloat32(), std::chars_format::scientific);
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Float64:
			{
				char buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetFloat64(), std::chars_format::scientific);
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::String:
			{
				stream.write("\"", 1);
				const std::string& value = node.GetString();
				const char* pos = std::strpbrk(value.data(), "\t\n\r\f\b\"\\");
				if (pos != nullptr)
				{
					std::string escapedValue = value;
					pos = std::strpbrk(escapedValue.data(), "\t\n\r\f\b\"\\");

					while (pos != nullptr)
					{
						switch (*pos)
						{
						case '\t': *const_cast<char*>(pos) = 't'; break;
						case '\n': *const_cast<char*>(pos) = 'n'; break;
						case '\r': *const_cast<char*>(pos) = 'r'; break;
						case '\f': *const_cast<char*>(pos) = 'f'; break;
						case '\b': *const_cast<char*>(pos) = 'b'; break;
						case '\"': *const_cast<char*>(pos) = '"'; break;
						case '\\': *const_cast<char*>(pos) = '\\'; break;
						default: assert(false); break;
						}
						size_t offset = pos - escapedValue.data();
						escapedValue.insert(offset, "\\");
						pos = std::strpbrk(escapedValue.data() + offset + 2, "\t\n\r\f\b\"\\");
					}
					stream.write(escapedValue.data(), escapedValue.size());
				}
				else
				{
					stream.write(value.data(), value.size());
				}
				stream.write("\"", 1);
			}
			break;
		
			default:
			break;
		}

		return true;
	}
}
