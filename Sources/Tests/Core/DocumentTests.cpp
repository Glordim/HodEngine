#include <gtest/gtest.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>

#include <limits>
#include <sstream>

#undef min
#undef max

namespace hod
{
	static const char* expectedJson = "{\"bools\":{\"true\":true,\"false\":false,\"array\":[true,false]},\"integers\":{\"uint8\":{\"min\":0,\"max\":255,\"array\":[0,255]},\"uint16\":{\"min\":0,\"max\":65535,\"array\":[0,65535]},\"uint32\":{\"min\":0,\"max\":4294967295,\"array\":[0,4294967295]},\"uint64\":{\"min\":0,\"max\":18446744073709551615,\"array\":[0,18446744073709551615]},\"int8\":{\"min\":-128,\"max\":127,\"array\":[-128,127]},\"int16\":{\"min\":-32768,\"max\":32767,\"array\":[-32768,32767]},\"int32\":{\"min\":-2147483648,\"max\":2147483647,\"array\":[-2147483648,2147483647]},\"int64\":{\"min\":-9223372036854775808,\"max\":9223372036854775807,\"array\":[-9223372036854775808,9223372036854775807]}},\"floatings\":{\"float32\":{\"min\":1.1754944e-38,\"max\":3.4028235e+38,\"array\":[1.1754944e-38,3.4028235e+38]},\"float64\":{\"min\":2.2250738585072014e-308,\"max\":1.7976931348623157e+308,\"array\":[2.2250738585072014e-308,1.7976931348623157e+308]}},\"strings\":{\"basic\":\"Hello World\",\"escaped\":\"a\\tb\\nc\\bd\\\"e\\rf\\fg\\\\h\",\"array\":[\"Item 0\",\"Item 1\"]},\"objects\":{\"toto\":{},\"tata\":{},\"array\":[{\"toto\":{},\"tata\":{}},{\"toto\":{},\"tata\":{}}]}}";

	void FillDocument(Document& document)
	{
		DocumentNode& boolsNode = document.GetRootNode().AddChild("bools");
		{
			boolsNode.AddChild("true").SetBool(true);
			boolsNode.AddChild("false").SetBool(false);
			DocumentNode& boolsArrayNode = boolsNode.AddChild("array");
			boolsArrayNode.AddChild("").SetBool(true);
			boolsArrayNode.AddChild("").SetBool(false);
		}

		DocumentNode& intergersNode = document.GetRootNode().AddChild("integers");
		{
			DocumentNode& uint8Node = intergersNode.AddChild("uint8");
			{
				uint8Node.AddChild("min").SetUInt8(std::numeric_limits<uint8_t>::min());
				uint8Node.AddChild("max").SetUInt8(std::numeric_limits<uint8_t>::max());
				DocumentNode& arrayNode = uint8Node.AddChild("array");
				arrayNode.AddChild("").SetUInt8(std::numeric_limits<uint8_t>::min());
				arrayNode.AddChild("").SetUInt8(std::numeric_limits<uint8_t>::max());
			}

			DocumentNode& uint16Node = intergersNode.AddChild("uint16");
			{
				uint16Node.AddChild("min").SetUInt16(std::numeric_limits<uint16_t>::min());
				uint16Node.AddChild("max").SetUInt16(std::numeric_limits<uint16_t>::max());
				DocumentNode& arrayNode = uint16Node.AddChild("array");
				arrayNode.AddChild("").SetUInt16(std::numeric_limits<uint16_t>::min());
				arrayNode.AddChild("").SetUInt16(std::numeric_limits<uint16_t>::max());
			}

			DocumentNode& uint32Node = intergersNode.AddChild("uint32");
			{
				uint32Node.AddChild("min").SetUInt32(std::numeric_limits<uint32_t>::min());
				uint32Node.AddChild("max").SetUInt32(std::numeric_limits<uint32_t>::max());
				DocumentNode& arrayNode = uint32Node.AddChild("array");
				arrayNode.AddChild("").SetUInt32(std::numeric_limits<uint32_t>::min());
				arrayNode.AddChild("").SetUInt32(std::numeric_limits<uint32_t>::max());
			}

			DocumentNode& uint64Node = intergersNode.AddChild("uint64");
			{
				uint64Node.AddChild("min").SetUInt64(std::numeric_limits<uint64_t>::min());
				uint64Node.AddChild("max").SetUInt64(std::numeric_limits<uint64_t>::max());
				DocumentNode& arrayNode = uint64Node.AddChild("array");
				arrayNode.AddChild("").SetUInt64(std::numeric_limits<uint64_t>::min());
				arrayNode.AddChild("").SetUInt64(std::numeric_limits<uint64_t>::max());
			}

			DocumentNode& int8Node = intergersNode.AddChild("int8");
			{
				int8Node.AddChild("min").SetInt8(std::numeric_limits<int8_t>::min());
				int8Node.AddChild("max").SetInt8(std::numeric_limits<int8_t>::max());
				DocumentNode& arrayNode = int8Node.AddChild("array");
				arrayNode.AddChild("").SetInt8(std::numeric_limits<int8_t>::min());
				arrayNode.AddChild("").SetInt8(std::numeric_limits<int8_t>::max());
			}

			DocumentNode& int16Node = intergersNode.AddChild("int16");
			{
				int16Node.AddChild("min").SetInt16(std::numeric_limits<int16_t>::min());
				int16Node.AddChild("max").SetInt16(std::numeric_limits<int16_t>::max());
				DocumentNode& arrayNode = int16Node.AddChild("array");
				arrayNode.AddChild("").SetInt16(std::numeric_limits<int16_t>::min());
				arrayNode.AddChild("").SetInt16(std::numeric_limits<int16_t>::max());
			}

			DocumentNode& int32Node = intergersNode.AddChild("int32");
			{
				int32Node.AddChild("min").SetInt32(std::numeric_limits<int32_t>::min());
				int32Node.AddChild("max").SetInt32(std::numeric_limits<int32_t>::max());
				DocumentNode& arrayNode = int32Node.AddChild("array");
				arrayNode.AddChild("").SetInt32(std::numeric_limits<int32_t>::min());
				arrayNode.AddChild("").SetInt32(std::numeric_limits<int32_t>::max());
			}

			DocumentNode& int64Node = intergersNode.AddChild("int64");
			{
				int64Node.AddChild("min").SetInt64(std::numeric_limits<int64_t>::min());
				int64Node.AddChild("max").SetInt64(std::numeric_limits<int64_t>::max());
				DocumentNode& arrayNode = int64Node.AddChild("array");
				arrayNode.AddChild("").SetInt64(std::numeric_limits<int64_t>::min());
				arrayNode.AddChild("").SetInt64(std::numeric_limits<int64_t>::max());
			}
		}

		DocumentNode& floatingsNode = document.GetRootNode().AddChild("floatings");
		{
			DocumentNode& float32Node = floatingsNode.AddChild("float32");
			{
				float32Node.AddChild("min").SetFloat32(std::numeric_limits<float>::min());
				float32Node.AddChild("max").SetFloat32(std::numeric_limits<float>::max());
				DocumentNode& arrayNode = float32Node.AddChild("array");
				arrayNode.AddChild("").SetFloat32(std::numeric_limits<float>::min());
				arrayNode.AddChild("").SetFloat32(std::numeric_limits<float>::max());
			}
			DocumentNode& float64Node = floatingsNode.AddChild("float64");
			{
				float64Node.AddChild("min").SetFloat64(std::numeric_limits<double>::min());
				float64Node.AddChild("max").SetFloat64(std::numeric_limits<double>::max());
				DocumentNode& arrayNode = float64Node.AddChild("array");
				arrayNode.AddChild("").SetFloat64(std::numeric_limits<double>::min());
				arrayNode.AddChild("").SetFloat64(std::numeric_limits<double>::max());
			}
		}

		DocumentNode& stringsNode = document.GetRootNode().AddChild("strings");
		{
			stringsNode.AddChild("basic").SetString("Hello World");
			stringsNode.AddChild("escaped").SetString("a\tb\nc\bd\"e\rf\fg\\h");
			DocumentNode& arrayNode = stringsNode.AddChild("array");
			arrayNode.AddChild("").SetString("Item 0");
			arrayNode.AddChild("").SetString("Item 1");
		}

		DocumentNode& objectsNode = document.GetRootNode().AddChild("objects");
		{
			objectsNode.AddChild("toto");
			objectsNode.AddChild("tata");
			DocumentNode& arrayNode = objectsNode.AddChild("array");
			DocumentNode& child0 = arrayNode.AddChild("");
			child0.AddChild("toto");
			child0.AddChild("tata");
			DocumentNode& child1 = arrayNode.AddChild("");
			child1.AddChild("toto");
			child1.AddChild("tata");
		}
	}

	void VerifyDocument(const Document& document)
	{
		const DocumentNode* boolsNode = document.GetRootNode().GetChild("bools");
		EXPECT_TRUE(boolsNode);
		{
			const DocumentNode* trueNode = boolsNode->GetChild("true");
			EXPECT_TRUE(trueNode);
			EXPECT_EQ(trueNode->GetType(), DocumentNode::Type::Bool);
			EXPECT_EQ(trueNode->GetBool(), true);

			const DocumentNode* falseNode = boolsNode->GetChild("false");
			EXPECT_TRUE(falseNode);
			EXPECT_EQ(falseNode->GetType(), DocumentNode::Type::Bool);
			EXPECT_EQ(falseNode->GetBool(), false);

			const DocumentNode* arrayNode = boolsNode->GetChild("array");
			EXPECT_TRUE(arrayNode);
			EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
			EXPECT_EQ(arrayNode->GetChildCount(), 2);
			const DocumentNode* child = arrayNode->GetFirstChild();
			EXPECT_TRUE(child);
			EXPECT_EQ(child->GetType(), DocumentNode::Type::Bool);
			EXPECT_EQ(child->GetBool(), true);
			child = child->GetNextSibling();
			EXPECT_TRUE(child);
			EXPECT_EQ(child->GetType(), DocumentNode::Type::Bool);
			EXPECT_EQ(child->GetBool(), false);
			child = child->GetNextSibling();
			EXPECT_FALSE(child);
		}

		const DocumentNode* intergersNode = document.GetRootNode().GetChild("integers");
		EXPECT_TRUE(intergersNode);
		{
			const DocumentNode* uint8Node = intergersNode->GetChild("uint8");
			EXPECT_TRUE(uint8Node);
			{
				const DocumentNode* minNode = uint8Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::UInt8);
				EXPECT_EQ(minNode->GetUInt8(), std::numeric_limits<uint8_t>::min());

				const DocumentNode* maxNode = uint8Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::UInt8);
				EXPECT_EQ(maxNode->GetUInt8(), std::numeric_limits<uint8_t>::max());

				const DocumentNode* arrayNode = uint8Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::UInt8);
				EXPECT_EQ(child->GetUInt8(), std::numeric_limits<uint8_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::UInt8);
				EXPECT_EQ(child->GetUInt8(), std::numeric_limits<uint8_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const DocumentNode* uint16Node = intergersNode->GetChild("uint16");
			EXPECT_TRUE(uint16Node);
			{
				const DocumentNode* minNode = uint16Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::UInt16);
				EXPECT_EQ(minNode->GetUInt16(), std::numeric_limits<uint16_t>::min());

				const DocumentNode* maxNode = uint16Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::UInt16);
				EXPECT_EQ(maxNode->GetUInt16(), std::numeric_limits<uint16_t>::max());

				const DocumentNode* arrayNode = uint16Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::UInt16);
				EXPECT_EQ(child->GetUInt16(), std::numeric_limits<uint16_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::UInt16);
				EXPECT_EQ(child->GetUInt16(), std::numeric_limits<uint16_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const DocumentNode* uint32Node = intergersNode->GetChild("uint32");
			EXPECT_TRUE(uint32Node);
			{
				const DocumentNode* minNode = uint32Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::UInt32);
				EXPECT_EQ(minNode->GetUInt32(), std::numeric_limits<uint32_t>::min());

				const DocumentNode* maxNode = uint32Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::UInt32);
				EXPECT_EQ(maxNode->GetUInt32(), std::numeric_limits<uint32_t>::max());

				const DocumentNode* arrayNode = uint32Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::UInt32);
				EXPECT_EQ(child->GetUInt32(), std::numeric_limits<uint32_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::UInt32);
				EXPECT_EQ(child->GetUInt32(), std::numeric_limits<uint32_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const DocumentNode* uint64Node = intergersNode->GetChild("uint64");
			EXPECT_TRUE(uint64Node);
			{
				const DocumentNode* minNode = uint64Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::UInt64);
				EXPECT_EQ(minNode->GetUInt64(), std::numeric_limits<uint64_t>::min());

				const DocumentNode* maxNode = uint64Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::UInt64);
				EXPECT_EQ(maxNode->GetUInt64(), std::numeric_limits<uint64_t>::max());

				const DocumentNode* arrayNode = uint64Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::UInt64);
				EXPECT_EQ(child->GetUInt64(), std::numeric_limits<uint64_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::UInt64);
				EXPECT_EQ(child->GetUInt64(), std::numeric_limits<uint64_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const DocumentNode* int8Node = intergersNode->GetChild("int8");
			EXPECT_TRUE(int8Node);
			{
				const DocumentNode* minNode = int8Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::Int8);
				EXPECT_EQ(minNode->GetInt8(), std::numeric_limits<int8_t>::min());

				const DocumentNode* maxNode = int8Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::Int8);
				EXPECT_EQ(maxNode->GetInt8(), std::numeric_limits<int8_t>::max());

				const DocumentNode* arrayNode = int8Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Int8);
				EXPECT_EQ(child->GetInt8(), std::numeric_limits<int8_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Int8);
				EXPECT_EQ(child->GetInt8(), std::numeric_limits<int8_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const DocumentNode* int16Node = intergersNode->GetChild("int16");
			EXPECT_TRUE(int16Node);
			{
				const DocumentNode* minNode = int16Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::Int16);
				EXPECT_EQ(minNode->GetInt16(), std::numeric_limits<int16_t>::min());

				const DocumentNode* maxNode = int16Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::Int16);
				EXPECT_EQ(maxNode->GetInt16(), std::numeric_limits<int16_t>::max());

				const DocumentNode* arrayNode = int16Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Int16);
				EXPECT_EQ(child->GetInt16(), std::numeric_limits<int16_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Int16);
				EXPECT_EQ(child->GetInt16(), std::numeric_limits<int16_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const DocumentNode* int32Node = intergersNode->GetChild("int32");
			EXPECT_TRUE(int32Node);
			{
				const DocumentNode* minNode = int32Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::Int32);
				EXPECT_EQ(minNode->GetInt32(), std::numeric_limits<int32_t>::min());

				const DocumentNode* maxNode = int32Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::Int32);
				EXPECT_EQ(maxNode->GetInt32(), std::numeric_limits<int32_t>::max());

				const DocumentNode* arrayNode = int32Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Int32);
				EXPECT_EQ(child->GetInt32(), std::numeric_limits<int32_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Int32);
				EXPECT_EQ(child->GetInt32(), std::numeric_limits<int32_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const DocumentNode* int64Node = intergersNode->GetChild("int64");
			EXPECT_TRUE(int64Node);
			{
				const DocumentNode* minNode = int64Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::Int64);
				EXPECT_EQ(minNode->GetInt64(), std::numeric_limits<int64_t>::min());

				const DocumentNode* maxNode = int64Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::Int64);
				EXPECT_EQ(maxNode->GetInt64(), std::numeric_limits<int64_t>::max());

				const DocumentNode* arrayNode = int64Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Int64);
				EXPECT_EQ(child->GetInt64(), std::numeric_limits<int64_t>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Int64);
				EXPECT_EQ(child->GetInt64(), std::numeric_limits<int64_t>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}
		}

		const DocumentNode* floatingsNode = document.GetRootNode().GetChild("floatings");
		EXPECT_TRUE(floatingsNode);
		{
			const DocumentNode* float32Node = floatingsNode->GetChild("float32");
			EXPECT_TRUE(float32Node);
			{
				const DocumentNode* minNode = float32Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::Float32);
				EXPECT_EQ(minNode->GetFloat32(), std::numeric_limits<float>::min());

				const DocumentNode* maxNode = float32Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::Float32);
				EXPECT_EQ(maxNode->GetFloat32(), std::numeric_limits<float>::max());

				const DocumentNode* arrayNode = float32Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Float32);
				EXPECT_EQ(child->GetFloat32(), std::numeric_limits<float>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Float32);
				EXPECT_EQ(child->GetFloat32(), std::numeric_limits<float>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}

			const DocumentNode* float64Node = floatingsNode->GetChild("float64");
			EXPECT_TRUE(float64Node);
			{
				const DocumentNode* minNode = float64Node->GetChild("min");
				EXPECT_TRUE(minNode);
				EXPECT_EQ(minNode->GetType(), DocumentNode::Type::Float64);
				EXPECT_EQ(minNode->GetFloat64(), std::numeric_limits<double>::min());

				const DocumentNode* maxNode = float64Node->GetChild("max");
				EXPECT_TRUE(maxNode);
				EXPECT_EQ(maxNode->GetType(), DocumentNode::Type::Float64);
				EXPECT_EQ(maxNode->GetFloat64(), std::numeric_limits<double>::max());

				const DocumentNode* arrayNode = float64Node->GetChild("array");
				EXPECT_TRUE(arrayNode);
				EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
				EXPECT_EQ(arrayNode->GetChildCount(), 2);
				const DocumentNode* child = arrayNode->GetFirstChild();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Float64);
				EXPECT_EQ(child->GetFloat64(), std::numeric_limits<double>::min());
				child = child->GetNextSibling();
				EXPECT_TRUE(child);
				EXPECT_EQ(child->GetType(), DocumentNode::Type::Float64);
				EXPECT_EQ(child->GetFloat64(), std::numeric_limits<double>::max());
				child = child->GetNextSibling();
				EXPECT_FALSE(child);
			}
		}

		const DocumentNode* stringsNode = document.GetRootNode().GetChild("strings");
		{
			const DocumentNode* basicNode = stringsNode->GetChild("basic");
			EXPECT_TRUE(basicNode);
			EXPECT_EQ(basicNode->GetType(), DocumentNode::Type::String);
			EXPECT_EQ(basicNode->GetString(), "Hello World");

			const DocumentNode* escapedNode = stringsNode->GetChild("escaped");
			EXPECT_TRUE(escapedNode);
			EXPECT_EQ(escapedNode->GetType(), DocumentNode::Type::String);
			EXPECT_EQ(escapedNode->GetString(), "a\tb\nc\bd\"e\rf\fg\\h");

			const DocumentNode* arrayNode = stringsNode->GetChild("array");
			EXPECT_TRUE(arrayNode);
			EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
			EXPECT_EQ(arrayNode->GetChildCount(), 2);
			const DocumentNode* child = arrayNode->GetFirstChild();
			EXPECT_TRUE(child);
			EXPECT_EQ(child->GetType(), DocumentNode::Type::String);
			EXPECT_EQ(child->GetString(), "Item 0");
			child = child->GetNextSibling();
			EXPECT_TRUE(child);
			EXPECT_EQ(child->GetType(), DocumentNode::Type::String);
			EXPECT_EQ(child->GetString(), "Item 1");
			child = child->GetNextSibling();
			EXPECT_FALSE(child);
		}

		const DocumentNode* objectsNode = document.GetRootNode().GetChild("objects");
		{
			const DocumentNode* totoNode = objectsNode->GetChild("toto");
			EXPECT_TRUE(totoNode);
			EXPECT_EQ(totoNode->GetType(), DocumentNode::Type::Object);

			const DocumentNode* tataNode = objectsNode->GetChild("tata");
			EXPECT_TRUE(tataNode);
			EXPECT_EQ(tataNode->GetType(), DocumentNode::Type::Object);

			const DocumentNode* arrayNode = objectsNode->GetChild("array");
			EXPECT_TRUE(arrayNode);
			EXPECT_EQ(arrayNode->GetType(), DocumentNode::Type::Array);
			EXPECT_EQ(arrayNode->GetChildCount(), 2);
			const DocumentNode* child = arrayNode->GetFirstChild();
			EXPECT_TRUE(child);
			EXPECT_EQ(child->GetType(), DocumentNode::Type::Object);
			child = child->GetNextSibling();
			EXPECT_TRUE(child);
			EXPECT_EQ(child->GetType(), DocumentNode::Type::Object);
			child = child->GetNextSibling();
			EXPECT_FALSE(child);
		}
	}

	TEST(Document, PopulateDocument)
	{
		Document document;
		FillDocument(document);

		VerifyDocument(document);
	}
	TEST(Document, JsonWriter)
	{
		Document document;
		FillDocument(document);

		std::stringstream output;

		DocumentWriterJson writer;
		EXPECT_TRUE(writer.Write(document, output));

		EXPECT_STREQ(output.str().c_str(), expectedJson);
	}

	TEST(Document, JsonReader)
	{
		Document document;
		DocumentReaderJson reader;
		EXPECT_TRUE(reader.Read(document, expectedJson, sizeof(expectedJson)));

		VerifyDocument(document);
	}
}
