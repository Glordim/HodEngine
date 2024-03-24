#include <gtest/gtest.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

#include <limits>
#include <sstream>

namespace hod
{
	// Demonstrate some basic assertions.
	TEST(Document, WritterJson)
	{
		Document document;
		Document::Node& intergersNode = document.GetRootNode().AddChild("integers");
		{
			Document::Node& uint8Node = intergersNode.AddChild("uint8");
			uint8Node.AddChild("min").SetUInt8(std::numeric_limits<uint8_t>::min());
			uint8Node.AddChild("max").SetUInt8(std::numeric_limits<uint8_t>::max());
			Document::Node& uint16Node = intergersNode.AddChild("uint16");
			uint16Node.AddChild("min").SetUInt16(std::numeric_limits<uint16_t>::min());
			uint16Node.AddChild("max").SetUInt16(std::numeric_limits<uint16_t>::max());
			Document::Node& uint32Node = intergersNode.AddChild("uint32");
			uint32Node.AddChild("min").SetUInt32(std::numeric_limits<uint32_t>::min());
			uint32Node.AddChild("max").SetUInt32(std::numeric_limits<uint32_t>::max());
			Document::Node& uint64Node = intergersNode.AddChild("uint64");
			uint64Node.AddChild("min").SetUInt64(std::numeric_limits<uint64_t>::min());
			uint64Node.AddChild("max").SetUInt64(std::numeric_limits<uint64_t>::max());

			Document::Node& int8Node = intergersNode.AddChild("int8");
			int8Node.AddChild("min").SetInt8(std::numeric_limits<int8_t>::min());
			int8Node.AddChild("max").SetInt8(std::numeric_limits<int8_t>::max());
			Document::Node& int16Node = intergersNode.AddChild("int16");
			int16Node.AddChild("min").SetInt16(std::numeric_limits<int16_t>::min());
			int16Node.AddChild("max").SetInt16(std::numeric_limits<int16_t>::max());
			Document::Node& int32Node = intergersNode.AddChild("int32");
			int32Node.AddChild("min").SetInt32(std::numeric_limits<int32_t>::min());
			int32Node.AddChild("max").SetInt32(std::numeric_limits<int32_t>::max());
			Document::Node& int64Node = intergersNode.AddChild("int64");
			int64Node.AddChild("min").SetInt64(std::numeric_limits<int64_t>::min());
			int64Node.AddChild("max").SetInt64(std::numeric_limits<int64_t>::max());
		}

		Document::Node& floatingsNode = document.GetRootNode().AddChild("floatings");
		{
			Document::Node& float32Node = floatingsNode.AddChild("float32");
			float32Node.AddChild("min").SetFloat32(std::numeric_limits<float>::min());
			float32Node.AddChild("max").SetFloat32(std::numeric_limits<float>::max());
			Document::Node& float64Node = floatingsNode.AddChild("float64");
			float64Node.AddChild("min").SetFloat64(std::numeric_limits<double>::min());
			float64Node.AddChild("max").SetFloat64(std::numeric_limits<double>::max());
		}

		document.GetRootNode().AddChild("string").SetString("a\tb\nc\bd\"e\rf\fg\\h");

		static const char* expectedJson = "{\"integers\":{\"uint8\":{\"min\":0,\"max\":255},\"uint16\":{\"min\":0,\"max\":65535},\"uint32\":{\"min\":0,\"max\":4294967295},\"uint64\":{\"min\":0,\"max\":18446744073709551615},\"int8\":{\"min\":-128,\"max\":127},\"int16\":{\"min\":-32768,\"max\":32767},\"int32\":{\"min\":-2147483648,\"max\":2147483647},\"int64\":{\"min\":-9223372036854775808,\"max\":9223372036854775807}},\"floatings\":{\"float32\":{\"min\":1.1754944e-38,\"max\":3.4028235e+38},\"float64\":{\"min\":2.2250738585072014e-308,\"max\":1.7976931348623157e+308}},\"string\":\"a\\tb\\nc\\bd\\\"e\\rf\\fg\\\\h\"}";

		std::stringstream output;

		DocumentWriterJson writter;
		writter.Write(document, output);

		EXPECT_STREQ(output.str().c_str(), expectedJson);
	}
}
