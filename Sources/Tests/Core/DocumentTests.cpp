#include <gtest/gtest.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

#include <limits>
#include <sstream>

namespace hod
{
	void FillDocument(Document& document)
	{
		Document::Node& boolsNode = document.GetRootNode().AddChild("bools");
		{
			boolsNode.AddChild("true").SetBool(true);
			boolsNode.AddChild("false").SetBool(false);
			Document::Node& boolsArrayNode = boolsNode.AddChild("array");
			boolsArrayNode.AddChild("").SetBool(true);
			boolsArrayNode.AddChild("").SetBool(false);
		}

		Document::Node& intergersNode = document.GetRootNode().AddChild("integers");
		{
			Document::Node& uint8Node = intergersNode.AddChild("uint8");
			{
				uint8Node.AddChild("min").SetUInt8(std::numeric_limits<uint8_t>::min());
				uint8Node.AddChild("max").SetUInt8(std::numeric_limits<uint8_t>::max());
				Document::Node& arrayNode = uint8Node.AddChild("array");
				arrayNode.AddChild("").SetUInt8(std::numeric_limits<uint8_t>::min());
				arrayNode.AddChild("").SetUInt8(std::numeric_limits<uint8_t>::max());
			}

			Document::Node& uint16Node = intergersNode.AddChild("uint16");
			{
				uint16Node.AddChild("min").SetUInt16(std::numeric_limits<uint16_t>::min());
				uint16Node.AddChild("max").SetUInt16(std::numeric_limits<uint16_t>::max());
				Document::Node& arrayNode = uint16Node.AddChild("array");
				arrayNode.AddChild("").SetUInt16(std::numeric_limits<uint16_t>::min());
				arrayNode.AddChild("").SetUInt16(std::numeric_limits<uint16_t>::max());
			}

			Document::Node& uint32Node = intergersNode.AddChild("uint32");
			{
				uint32Node.AddChild("min").SetUInt32(std::numeric_limits<uint32_t>::min());
				uint32Node.AddChild("max").SetUInt32(std::numeric_limits<uint32_t>::max());
				Document::Node& arrayNode = uint32Node.AddChild("array");
				arrayNode.AddChild("").SetUInt32(std::numeric_limits<uint32_t>::min());
				arrayNode.AddChild("").SetUInt32(std::numeric_limits<uint32_t>::max());
			}

			Document::Node& uint64Node = intergersNode.AddChild("uint64");
			{
				uint64Node.AddChild("min").SetUInt64(std::numeric_limits<uint64_t>::min());
				uint64Node.AddChild("max").SetUInt64(std::numeric_limits<uint64_t>::max());
				Document::Node& arrayNode = uint64Node.AddChild("array");
				arrayNode.AddChild("").SetUInt64(std::numeric_limits<uint64_t>::min());
				arrayNode.AddChild("").SetUInt64(std::numeric_limits<uint64_t>::max());
			}

			Document::Node& int8Node = intergersNode.AddChild("int8");
			{
				int8Node.AddChild("min").SetInt8(std::numeric_limits<int8_t>::min());
				int8Node.AddChild("max").SetInt8(std::numeric_limits<int8_t>::max());
				Document::Node& arrayNode = int8Node.AddChild("array");
				arrayNode.AddChild("").SetInt8(std::numeric_limits<int8_t>::min());
				arrayNode.AddChild("").SetInt8(std::numeric_limits<int8_t>::max());
			}

			Document::Node& int16Node = intergersNode.AddChild("int16");
			{
				int16Node.AddChild("min").SetInt16(std::numeric_limits<int16_t>::min());
				int16Node.AddChild("max").SetInt16(std::numeric_limits<int16_t>::max());
				Document::Node& arrayNode = int16Node.AddChild("array");
				arrayNode.AddChild("").SetInt16(std::numeric_limits<int16_t>::min());
				arrayNode.AddChild("").SetInt16(std::numeric_limits<int16_t>::max());
			}

			Document::Node& int32Node = intergersNode.AddChild("int32");
			{
				int32Node.AddChild("min").SetInt32(std::numeric_limits<int32_t>::min());
				int32Node.AddChild("max").SetInt32(std::numeric_limits<int32_t>::max());
				Document::Node& arrayNode = int32Node.AddChild("array");
				arrayNode.AddChild("").SetInt32(std::numeric_limits<int32_t>::min());
				arrayNode.AddChild("").SetInt32(std::numeric_limits<int32_t>::max());
			}

			Document::Node& int64Node = intergersNode.AddChild("int64");
			{
				int64Node.AddChild("min").SetInt64(std::numeric_limits<int64_t>::min());
				int64Node.AddChild("max").SetInt64(std::numeric_limits<int64_t>::max());
				Document::Node& arrayNode = int64Node.AddChild("array");
				arrayNode.AddChild("").SetInt64(std::numeric_limits<int64_t>::min());
				arrayNode.AddChild("").SetInt64(std::numeric_limits<int64_t>::max());
			}
		}

		Document::Node& floatingsNode = document.GetRootNode().AddChild("floatings");
		{
			Document::Node& float32Node = floatingsNode.AddChild("float32");
			{
				float32Node.AddChild("min").SetFloat32(std::numeric_limits<float>::min());
				float32Node.AddChild("max").SetFloat32(std::numeric_limits<float>::max());
				Document::Node& arrayNode = float32Node.AddChild("array");
				arrayNode.AddChild("").SetFloat32(std::numeric_limits<float>::min());
				arrayNode.AddChild("").SetFloat32(std::numeric_limits<float>::max());
			}
			Document::Node& float64Node = floatingsNode.AddChild("float64");
			{
				float64Node.AddChild("min").SetFloat64(std::numeric_limits<double>::min());
				float64Node.AddChild("max").SetFloat64(std::numeric_limits<double>::max());
				Document::Node& arrayNode = float64Node.AddChild("array");
				arrayNode.AddChild("").SetFloat64(std::numeric_limits<double>::min());
				arrayNode.AddChild("").SetFloat64(std::numeric_limits<double>::max());
			}
		}

		Document::Node& stringsNode = document.GetRootNode().AddChild("strings");
		{
			stringsNode.AddChild("basic").SetString("Hello World");
			stringsNode.AddChild("escaped").SetString("a\tb\nc\bd\"e\rf\fg\\h");
			Document::Node& arrayNode = stringsNode.AddChild("array");
			arrayNode.AddChild("").SetString("Item 0");
			arrayNode.AddChild("").SetString("Item 1");
		}

		Document::Node& objectsNode = document.GetRootNode().AddChild("objects");
		{
			objectsNode.AddChild("toto");
			objectsNode.AddChild("tata");
			Document::Node& arrayNode = objectsNode.AddChild("array");
			Document::Node& child0 = arrayNode.AddChild("");
			child0.AddChild("toto");
			child0.AddChild("tata");
			Document::Node& child1 = arrayNode.AddChild("");
			child1.AddChild("toto");
			child1.AddChild("tata");
		}
	}

	TEST(Document, AddChild)
	{
		Document document;
		FillDocument(document);

		const Document::Node* boolsNode = document.GetRootNode().GetChild("bools");
		EXPECT_TRUE(boolsNode);
		{
			const Document::Node* trueNode = boolsNode->GetChild("true");
			EXPECT_TRUE(trueNode);
			EXPECT_TRUE(trueNode->GetType() == Document::Node::Type::Bool);
			EXPECT_TRUE(trueNode->GetBool() == true);

			const Document::Node* falseNode = boolsNode->GetChild("false");
			EXPECT_TRUE(falseNode);
			EXPECT_TRUE(falseNode->GetType() == Document::Node::Type::Bool);
			EXPECT_TRUE(falseNode->GetBool() == false);

			const Document::Node* arrayNode = boolsNode->GetChild("array");
			EXPECT_TRUE(arrayNode);
			EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
			EXPECT_TRUE(arrayNode->GetChildCount() == 2);
			const Document::Node* child = arrayNode->GetFirstChild();
			EXPECT_TRUE(child);
			EXPECT_TRUE(child->GetType() == Document::Node::Type::Bool);
			EXPECT_TRUE(child->GetBool() == true);
			child = child->GetNextSibling();
			EXPECT_TRUE(child);
			EXPECT_TRUE(child->GetType() == Document::Node::Type::Bool);
			EXPECT_TRUE(child->GetBool() == false);
			child = child->GetNextSibling();
			EXPECT_FALSE(child);
		}

		const Document::Node* intergersNode = document.GetRootNode().GetChild("integers");
		EXPECT_TRUE(intergersNode);
		{
			const Document::Node* uint8Node = intergersNode->GetChild("uint8");
			EXPECT_TRUE(uint8Node);
			{
				const Document::Node* minNode = uint8Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::UInt8);
				EXPECT_TRUE(minNode->GetUInt8() == std::numeric_limits<uint8_t>::min());

				const Document::Node* maxNode = uint8Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::UInt8);
				EXPECT_TRUE(maxNode->GetUInt8() == std::numeric_limits<uint8_t>::max());

				const Document::Node* arrayNode = uint8Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::UInt8);
				EXPECT_TRUE(child->GetUInt8() == std::numeric_limits<uint8_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::UInt8);
				EXPECT_TRUE(child->GetUInt8() == std::numeric_limits<uint8_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const Document::Node* uint16Node = intergersNode->GetChild("uint16");
			EXPECT_TRUE(uint16Node);
			{
				const Document::Node* minNode = uint16Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::UInt16);
				EXPECT_TRUE(minNode->GetUInt16() == std::numeric_limits<uint16_t>::min());

				const Document::Node* maxNode = uint16Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::UInt16);
				EXPECT_TRUE(maxNode->GetUInt16() == std::numeric_limits<uint16_t>::max());

				const Document::Node* arrayNode = uint16Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::UInt16);
				EXPECT_TRUE(child->GetUInt16() == std::numeric_limits<uint16_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::UInt16);
				EXPECT_TRUE(child->GetUInt16() == std::numeric_limits<uint16_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const Document::Node* uint32Node = intergersNode->GetChild("uint32");
			EXPECT_TRUE(uint32Node);
			{
				const Document::Node* minNode = uint32Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::UInt32);
				EXPECT_TRUE(minNode->GetUInt32() == std::numeric_limits<uint32_t>::min());

				const Document::Node* maxNode = uint32Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::UInt32);
				EXPECT_TRUE(maxNode->GetUInt32() == std::numeric_limits<uint32_t>::max());

				const Document::Node* arrayNode = uint32Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::UInt32);
				EXPECT_TRUE(child->GetUInt32() == std::numeric_limits<uint32_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::UInt32);
				EXPECT_TRUE(child->GetUInt32() == std::numeric_limits<uint32_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const Document::Node* uint64Node = intergersNode->GetChild("uint64");
			EXPECT_TRUE(uint64Node);
			{
				const Document::Node* minNode = uint64Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::UInt64);
				EXPECT_TRUE(minNode->GetUInt64() == std::numeric_limits<uint64_t>::min());

				const Document::Node* maxNode = uint64Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::UInt64);
				EXPECT_TRUE(maxNode->GetUInt64() == std::numeric_limits<uint64_t>::max());

				const Document::Node* arrayNode = uint64Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::UInt64);
				EXPECT_TRUE(child->GetUInt64() == std::numeric_limits<uint64_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::UInt64);
				EXPECT_TRUE(child->GetUInt64() == std::numeric_limits<uint64_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const Document::Node* int8Node = intergersNode->GetChild("int8");
			EXPECT_TRUE(int8Node);
			{
				const Document::Node* minNode = int8Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::Int8);
				EXPECT_TRUE(minNode->GetInt8() == std::numeric_limits<int8_t>::min());

				const Document::Node* maxNode = int8Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::Int8);
				EXPECT_TRUE(maxNode->GetInt8() == std::numeric_limits<int8_t>::max());

				const Document::Node* arrayNode = int8Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Int8);
				EXPECT_TRUE(child->GetInt8() == std::numeric_limits<int8_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Int8);
				EXPECT_TRUE(child->GetInt8() == std::numeric_limits<int8_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const Document::Node* int16Node = intergersNode->GetChild("int16");
			EXPECT_TRUE(int16Node);
			{
				const Document::Node* minNode = int16Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::Int16);
				EXPECT_TRUE(minNode->GetInt16() == std::numeric_limits<int16_t>::min());

				const Document::Node* maxNode = int16Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::Int16);
				EXPECT_TRUE(maxNode->GetInt16() == std::numeric_limits<int16_t>::max());

				const Document::Node* arrayNode = int16Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Int16);
				EXPECT_TRUE(child->GetInt16() == std::numeric_limits<int16_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Int16);
				EXPECT_TRUE(child->GetInt16() == std::numeric_limits<int16_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const Document::Node* int32Node = intergersNode->GetChild("int32");
			EXPECT_TRUE(int32Node);
			{
				const Document::Node* minNode = int32Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::Int32);
				EXPECT_TRUE(minNode->GetInt32() == std::numeric_limits<int32_t>::min());

				const Document::Node* maxNode = int32Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::Int32);
				EXPECT_TRUE(maxNode->GetInt32() == std::numeric_limits<int32_t>::max());

				const Document::Node* arrayNode = int32Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Int32);
				EXPECT_TRUE(child->GetInt32() == std::numeric_limits<int32_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Int32);
				EXPECT_TRUE(child->GetInt32() == std::numeric_limits<int32_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const Document::Node* int64Node = intergersNode->GetChild("int64");
			EXPECT_TRUE(int64Node);
			{
				const Document::Node* minNode = int64Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::Int64);
				EXPECT_TRUE(minNode->GetInt64() == std::numeric_limits<int64_t>::min());

				const Document::Node* maxNode = int64Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::Int64);
				EXPECT_TRUE(maxNode->GetInt64() == std::numeric_limits<int64_t>::max());

				const Document::Node* arrayNode = int64Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Int64);
				EXPECT_TRUE(child->GetInt64() == std::numeric_limits<int64_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Int64);
				EXPECT_TRUE(child->GetInt64() == std::numeric_limits<int64_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}
		}

		const Document::Node* floatingsNode = document.GetRootNode().GetChild("floatings");
		EXPECT_TRUE(floatingsNode);
		{
			const Document::Node* float32Node = floatingsNode->GetChild("float32");
			EXPECT_TRUE(float32Node);
			{
				const Document::Node* minNode = float32Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::Float32);
				EXPECT_TRUE(minNode->GetFloat32() == std::numeric_limits<float>::min());

				const Document::Node* maxNode = float32Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::Float32);
				EXPECT_TRUE(maxNode->GetFloat32() == std::numeric_limits<float>::max());

				const Document::Node* arrayNode = float32Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Float32);
				EXPECT_TRUE(child->GetFloat32() == std::numeric_limits<float>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Float32);
				EXPECT_TRUE(child->GetFloat32() == std::numeric_limits<float>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const Document::Node* float64Node = floatingsNode->GetChild("float64");
			EXPECT_TRUE(float64Node);
			{
				const Document::Node* minNode = float64Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_TRUE(minNode->GetType() == Document::Node::Type::Float64);
				EXPECT_TRUE(minNode->GetFloat64() == std::numeric_limits<double>::min());

				const Document::Node* maxNode = float64Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_TRUE(maxNode->GetType() == Document::Node::Type::Float64);
				EXPECT_TRUE(maxNode->GetFloat64() == std::numeric_limits<double>::max());

				const Document::Node* arrayNode = float64Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
				EXPECT_TRUE(arrayNode->GetChildCount() == 2);
				const Document::Node* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Float64);
				EXPECT_TRUE(child->GetFloat64() == std::numeric_limits<double>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_TRUE(child->GetType() == Document::Node::Type::Float64);
				EXPECT_TRUE(child->GetFloat64() == std::numeric_limits<double>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}
		}

		const Document::Node* stringsNode = document.GetRootNode().GetChild("strings");
		{
			const Document::Node* basicNode = stringsNode->GetChild("basic");
			EXPECT_TRUE(basicNode);
			EXPECT_TRUE(basicNode->GetType() == Document::Node::Type::String);
			EXPECT_TRUE(basicNode->GetString() == "Hello World");

			const Document::Node* escapedNode = stringsNode->GetChild("escaped");
			EXPECT_TRUE(escapedNode);
			EXPECT_TRUE(escapedNode->GetType() == Document::Node::Type::String);
			EXPECT_TRUE(escapedNode->GetString() == "a\tb\nc\bd\"e\rf\fg\\h");

			const Document::Node* arrayNode = stringsNode->GetChild("array");
			EXPECT_TRUE(arrayNode);
			EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
			EXPECT_TRUE(arrayNode->GetChildCount() == 2);
			const Document::Node* child = arrayNode->GetFirstChild();
			EXPECT_TRUE(child);
			EXPECT_TRUE(child->GetType() == Document::Node::Type::String);
			EXPECT_TRUE(child->GetString() == "Item 0");
			child = child->GetNextSibling();
			EXPECT_TRUE(child);
			EXPECT_TRUE(child->GetType() == Document::Node::Type::String);
			EXPECT_TRUE(child->GetString() == "Item 1");
			child = child->GetNextSibling();
			EXPECT_FALSE(child);
		}

		const Document::Node* objectsNode = document.GetRootNode().GetChild("objects");
		{
			const Document::Node* totoNode = objectsNode->GetChild("toto");
			EXPECT_TRUE(totoNode);
			EXPECT_TRUE(totoNode->GetType() == Document::Node::Type::Object);

			const Document::Node* tataNode = objectsNode->GetChild("tata");
			EXPECT_TRUE(tataNode);
			EXPECT_TRUE(tataNode->GetType() == Document::Node::Type::Object);

			const Document::Node* arrayNode = objectsNode->GetChild("array");
			EXPECT_TRUE(arrayNode);
			EXPECT_TRUE(arrayNode->GetType() == Document::Node::Type::Array);
			EXPECT_TRUE(arrayNode->GetChildCount() == 2);
			const Document::Node* child = arrayNode->GetFirstChild();
			EXPECT_TRUE(child);
			EXPECT_TRUE(child->GetType() == Document::Node::Type::Object);
			child = child->GetNextSibling();
			EXPECT_TRUE(child);
			EXPECT_TRUE(child->GetType() == Document::Node::Type::Object);
			child = child->GetNextSibling();
			EXPECT_FALSE(child);
		}
	}

	TEST(Document, WritterJson)
	{
		Document document;
		FillDocument(document);

		static const char* expectedJson = "{\"bools\":{\"true\":true,\"false\":false,\"array\":[true,false]},\"integers\":{\"uint8\":{\"min\":0,\"max\":255,\"array\":[0,255]},\"uint16\":{\"min\":0,\"max\":65535,\"array\":[0,65535]},\"uint32\":{\"min\":0,\"max\":4294967295,\"array\":[0,4294967295]},\"uint64\":{\"min\":0,\"max\":18446744073709551615,\"array\":[0,18446744073709551615]},\"int8\":{\"min\":-128,\"max\":127,\"array\":[-128,127]},\"int16\":{\"min\":-32768,\"max\":32767,\"array\":[-32768,32767]},\"int32\":{\"min\":-2147483648,\"max\":2147483647,\"array\":[-2147483648,2147483647]},\"int64\":{\"min\":-9223372036854775808,\"max\":9223372036854775807,\"array\":[-9223372036854775808,9223372036854775807]}},\"floatings\":{\"float32\":{\"min\":1.1754944e-38,\"max\":3.4028235e+38,\"array\":[1.1754944e-38,3.4028235e+38]},\"float64\":{\"min\":2.2250738585072014e-308,\"max\":1.7976931348623157e+308,\"array\":[2.2250738585072014e-308,1.7976931348623157e+308]}},\"strings\":{\"basic\":\"Hello World\",\"escaped\":\"a\\tb\\nc\\bd\\\"e\\rf\\fg\\\\h\",\"array\":[\"Item 0\",\"Item 1\"]},\"objects\":{\"toto\":{},\"tata\":{},\"array\":[{\"toto\":{},\"tata\":{}},{\"toto\":{},\"tata\":{}}]}}";

		std::stringstream output;

		DocumentWriterJson writter;
		writter.Write(document, output);

		EXPECT_STREQ(output.str().c_str(), expectedJson);
	}
}
