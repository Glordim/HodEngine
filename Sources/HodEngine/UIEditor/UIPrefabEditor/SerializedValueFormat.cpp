#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/SerializedValueFormat.hpp"

#include <HodEngine/Core/Document/Document.hpp>

#include <cstdio>

namespace hod::inline editor
{
	/// @brief
	/// @param node
	/// @return
	String FormatSerializedValue(const DocumentNode& node)
	{
		char buffer[64] = {};
		switch (node.GetType())
		{
			case DocumentNode::Type::Object:
			case DocumentNode::Type::Array:
			{
				String text = "(";
				for (const DocumentNode* child = node.GetFirstChild(); child != nullptr; child = child->GetNextSibling())
				{
					if (child != node.GetFirstChild())
					{
						text += ", ";
					}
					text += FormatSerializedValue(*child);
				}
				return text + ")";
			}
			case DocumentNode::Type::Bool: return node.GetBool() ? "true" : "false";
			case DocumentNode::Type::Int8: std::snprintf(buffer, sizeof(buffer), "%d", (int)node.GetInt8()); break;
			case DocumentNode::Type::Int16: std::snprintf(buffer, sizeof(buffer), "%d", (int)node.GetInt16()); break;
			case DocumentNode::Type::Int32: std::snprintf(buffer, sizeof(buffer), "%d", (int)node.GetInt32()); break;
			case DocumentNode::Type::Int64: std::snprintf(buffer, sizeof(buffer), "%lld", (long long)node.GetInt64()); break;
			case DocumentNode::Type::UInt8: std::snprintf(buffer, sizeof(buffer), "%u", (unsigned)node.GetUInt8()); break;
			case DocumentNode::Type::UInt16: std::snprintf(buffer, sizeof(buffer), "%u", (unsigned)node.GetUInt16()); break;
			case DocumentNode::Type::UInt32: std::snprintf(buffer, sizeof(buffer), "%u", (unsigned)node.GetUInt32()); break;
			case DocumentNode::Type::UInt64: std::snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)node.GetUInt64()); break;
			case DocumentNode::Type::Float32: std::snprintf(buffer, sizeof(buffer), "%.3g", (double)node.GetFloat32()); break;
			case DocumentNode::Type::Float64: std::snprintf(buffer, sizeof(buffer), "%.3g", node.GetFloat64()); break;
			case DocumentNode::Type::String: return String("\"") + node.GetString() + "\"";
		}
		return buffer;
	}
}
