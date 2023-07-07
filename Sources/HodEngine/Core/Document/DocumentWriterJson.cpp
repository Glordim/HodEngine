#include "HodEngine/Core/Document/DocumentWriterJson.h"
#include "HodEngine/Core/Stream/Stream.h"

#include <algorithm>

namespace hod
{
	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool DocumentWriterJson::WriteDocument(Document& document, Stream& stream)
	{
		return WriteNode(document.GetRootNode(), stream);
	}

	/// @brief 
	/// @param node 
	/// @param stream 
	/// @return 
	bool DocumentWriterJson::WriteNode(const Document::Node& node, Stream& stream)
	{
		const std::string& name = node.GetName();
		if (name.size() > 0)
		{
			stream.Write((void*)"\"", 1);
			stream.Write((void*)name.data(), name.size());
			stream.Write((void*)"\":", 2);
		}

		switch (node.GetType())
		{
			case Document::Node::Type::Object:
			{
				stream.Write((void*)"{", 1);
				Document::Node* child = node.GetFirstChild();
				while (child != nullptr)
				{
					WriteNode(*child, stream);
					child = child->GetNextSibling();
					if (child != nullptr)
					{
						stream.Write((void*)",", 1);
					}
				}
				stream.Write((void*)"}", 1);
			}
			break;

			case Document::Node::Type::Array:
			{
				stream.Write((void*)"[", 1);
				Document::Node* child = node.GetFirstChild();
				while (child != nullptr)
				{
					WriteNode(*child, stream);
					child = child->GetNextSibling();
					if (child != nullptr)
					{
						stream.Write((void*)",", 1);
					}
				}
				stream.Write((void*)"]", 1);
			}
			break;

			case Document::Node::Type::Bool:
			{
				if (node.GetBool() == true)
				{
					stream.Write((void*)"true", 4);
				}
				else
				{
					stream.Write((void*)"false", 5);
				}
			}
			break;

			case Document::Node::Type::SInt8:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%i", node.GetInt8());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::SInt16:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%i", node.GetInt16());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::SInt32:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%i", node.GetInt32());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::SInt64:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%lli", node.GetInt64());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::UInt8:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%u", node.GetUInt8());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::UInt16:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%u", node.GetUInt16());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::UInt32:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%u", node.GetUInt32());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::UInt64:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%llu", node.GetUInt64());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::Float32:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%f", node.GetFloat32());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::Float64:
			{
				char buffer[4096] = { '\0' };
				int bufferSize = std::sprintf(buffer, "%lf", node.GetFloat64());
				stream.Write(buffer, bufferSize);
			}
			break;

			case Document::Node::Type::String:
			{
				stream.Write((void*)"\"", 1);
				const std::string& value = node.GetString();
				const char* pos = std::strpbrk(value.data(), "\t\n\r\f\b\"\\");
				if (pos != nullptr)
				{
					std::string escapedValue = value;
					pos = std::strpbrk(escapedValue.data(), "\t\n\r\f\b\"\\");

					while (pos != nullptr)
					{
						uint32_t offset = pos - escapedValue.data();
						escapedValue.insert(offset, "\\");
						pos = std::strpbrk(escapedValue.data() + offset + 2, "\t\n\r\f\b\"\\");
					}
					stream.Write((void*)escapedValue.data(), escapedValue.size());
				}
				else
				{
					stream.Write((void*)value.data(), value.size());
				}
				stream.Write((void*)"\"", 1);
			}
			break;
		
			default:
			break;
		}

		return true;
	}
}
