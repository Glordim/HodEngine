#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"

#include <cassert>
#include <charconv>
#include <cstring>

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
		if (_pretty == true)
		{
			for (uint32_t i = 0; i < _indentation; ++i)
			{
				stream.write("\t", 1);
			}
		}

		const String& name = node.GetName();
		if (name.Size() > 0)
		{
			stream.write("\"", 1);
			stream.write(name.CStr(), name.Size());
			stream.write("\":", 2);

			if (_pretty == true)
			{
				stream.write(" ", 1);
			}
		}

		switch (node.GetType())
		{
			case Document::Node::Type::Object:
			{
				Document::Node* child = node.GetFirstChild();
				if (child == nullptr)
				{
					stream.write("{}", 2);
				}
				else
				{
					stream.write("{", 1);
					if (_pretty == true)
					{
						++_indentation;
						stream.write("\n", 1);
					}
					while (child != nullptr)
					{
						WriteNode(*child, stream);
						child = child->GetNextSibling();
						if (child != nullptr)
						{
							stream.write(",", 1);
							if (_pretty == true)
							{
								stream.write("\n", 1);
							}
						}
					}
					if (_pretty == true)
					{
						--_indentation;
						stream.write("\n", 1);
						for (uint32_t i = 0; i < _indentation; ++i)
						{
							stream.write("\t", 1);
						}
					}
					stream.write("}", 1);
				}
			}
			break;

			case Document::Node::Type::Array:
			{
				Document::Node* child = node.GetFirstChild();
				if (child == nullptr)
				{
					stream.write("[]", 2);
				}
				else
				{
					stream.write("[", 1);
					if (_pretty == true)
					{
						++_indentation;
						stream.write("\n", 1);
					}
					while (child != nullptr)
					{
						WriteNode(*child, stream);
						child = child->GetNextSibling();
						if (child != nullptr)
						{
							stream.write(",", 1);
							if (_pretty == true)
							{
								stream.write("\n", 1);
							}
						}
					}
					if (_pretty == true)
					{
						--_indentation;
						stream.write("\n", 1);
						for (uint32_t i = 0; i < _indentation; ++i)
						{
							stream.write("\t", 1);
						}
					}
					stream.write("]", 1);
				}
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
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetInt8());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Int16:
			{
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetInt16());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Int32:
			{
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetInt32());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Int64:
			{
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetInt64());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::UInt8:
			{
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetUInt8());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::UInt16:
			{
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetUInt16());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::UInt32:
			{
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetUInt32());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::UInt64:
			{
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetUInt64());
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Float32:
			{
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetFloat32(), std::chars_format::scientific);
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::Float64:
			{
				char                 buffer[256];
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), node.GetFloat64(), std::chars_format::scientific);
				assert(result.ec == std::errc());
				stream.write(buffer, result.ptr - buffer);
			}
			break;

			case Document::Node::Type::String:
			{
				stream.write("\"", 1);
				const String& value = node.GetString();
				const char*   pos = std::strpbrk(value.CStr(), "\t\n\r\f\b\"\\");
				if (pos != nullptr)
				{
					String escapedValue = value;
					pos = std::strpbrk(escapedValue.CStr(), "\t\n\r\f\b\"\\");

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
						size_t offset = pos - escapedValue.CStr();
						escapedValue.Insert(offset, "\\");
						pos = std::strpbrk(escapedValue.CStr() + offset + 2, "\t\n\r\f\b\"\\");
					}
					stream.write(escapedValue.CStr(), escapedValue.Size());
				}
				else
				{
					stream.write(value.CStr(), value.Size());
				}
				stream.write("\"", 1);
			}
			break;

			default: break;
		}

		return true;
	}
}
