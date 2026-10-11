#include <gtest/gtest.h>

#include <HodEngine/RHI/Shader.hpp>
#include <HodEngine/RHI/ShaderConstantDescriptor.hpp>
#include <HodEngine/RHI/ShaderSetDescriptor.hpp>

#include <HodEngine/Core/Output/OutputBucket.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace hod;

// ============================================================================
// The reflection of one shader, as slangc emits it for each target.
// The fixtures are the fragment stage of Renderer/Shader/P2fT2f_Texture_Unlit.slang:
//   slangc P2fT2f_Texture_Unlit.slang -I ../ShaderLib -stage fragment -entry FragmentMain -reflection-json <fixture> -o <any>
//     -target spirv -profile glsl_450+spirv_1_3    (vulkan)
//     -target metal                                (metal)
//     -target hlsl -profile sm_6_0                 (d3d12)
// It declares a push constant "pc" (marked [PushConstant]), then a ParameterBlock made of, in this order:
// a uniform block "ubo", a texture "image" and its sampler "imageSampler".
// ============================================================================

namespace
{
	// Shader with no GPU side: only runs the common reflection parsing
	class ReflectedShader : public Shader
	{
	public:
		ReflectedShader()
		: Shader(ShaderType::Fragment)
		{
		}

		bool LoadFromIR(const void* /*bytecode*/, uint32_t /*bytecodeSize*/, const char* reflection, uint32_t reflectionSize) override
		{
			return GenerateDescriptors(reflection, reflectionSize);
		}
	};

	std::string ReadFixture(const char* name)
	{
		std::ifstream     file(std::string(TESTS_RENDERER_FIXTURES_DIR) + "/Reflection/" + name, std::ios::binary);
		std::stringstream content;
		content << file.rdbuf();
		return content.str();
	}

	const ShaderSetDescriptor::BlockUbo* FindUbo(const ShaderSetDescriptor& setDescriptor, const char* name)
	{
		for (const ShaderSetDescriptor::BlockUbo& ubo : setDescriptor.GetUboBlocks())
		{
			if (ubo._name == name)
			{
				return &ubo;
			}
		}
		return nullptr;
	}

	const ShaderSetDescriptor::BlockTexture* FindTexture(const ShaderSetDescriptor& setDescriptor, const char* name)
	{
		for (const ShaderSetDescriptor::BlockTexture& texture : setDescriptor.GetTextureBlocks())
		{
			if (texture._name == name)
			{
				return &texture;
			}
		}
		return nullptr;
	}
}

class ShaderReflection : public ::testing::Test
{
protected:
	void Load(const char* fixture)
	{
		std::string reflection = ReadFixture(fixture);
		ASSERT_FALSE(reflection.empty()) << "missing fixture " << fixture;
		ASSERT_TRUE(_shader.LoadFromIR(nullptr, 0, reflection.c_str(), (uint32_t)reflection.size()));

		ASSERT_EQ(_shader.GetSetDescriptors().size(), 1u);
		ASSERT_EQ(_shader.GetSetDescriptors().begin()->first, 0u);
		_set = _shader.GetSetDescriptors().begin()->second;
	}

	// Same on every target: the layout of the uniform block comes from the type, not from the binding
	void ExpectUboLayout()
	{
		const ShaderSetDescriptor::BlockUbo* ubo = FindUbo(*_set, "ubo");
		ASSERT_NE(ubo, nullptr);
		EXPECT_EQ(ubo->_rootMember._size, 16u);

		auto color = ubo->_rootMember._childsMap.find("color");
		ASSERT_NE(color, ubo->_rootMember._childsMap.end());
		EXPECT_EQ(color->second._memberType, ShaderSetDescriptor::BlockUbo::MemberType::Float4);
		EXPECT_EQ(color->second._offset, 0u);
		EXPECT_EQ(color->second._size, 16u);
	}

	void ExpectPushConstant()
	{
		ASSERT_NE(_shader.GetConstantDescriptor(), nullptr);
		EXPECT_EQ(_shader.GetConstantDescriptor()->GetSize(), 64u);
		EXPECT_EQ(_shader.GetConstantDescriptor()->GetShaderType(), Shader::ShaderType::Fragment);
	}

	// The blocks are in the order they are declared, whatever their bindings
	void ExpectTextureBlocks(uint32_t imageBinding, uint32_t imageSamplerBinding)
	{
		const Vector<ShaderSetDescriptor::BlockTexture>& textureBlocks = _set->GetTextureBlocks();
		ASSERT_EQ(textureBlocks.Size(), 2u);

		EXPECT_TRUE(textureBlocks[0]._name == "image");
		EXPECT_EQ(textureBlocks[0]._type, ShaderSetDescriptor::BlockTexture::Texture);
		EXPECT_EQ(textureBlocks[0]._binding, imageBinding);

		EXPECT_TRUE(textureBlocks[1]._name == "imageSampler");
		EXPECT_EQ(textureBlocks[1]._type, ShaderSetDescriptor::BlockTexture::Sampler);
		EXPECT_EQ(textureBlocks[1]._binding, imageSamplerBinding);
	}

protected:
	ReflectedShader            _shader;
	const ShaderSetDescriptor* _set = nullptr;
};

// One numbering for everything in the set, and a dedicated push constant
TEST_F(ShaderReflection, Vulkan)
{
	Load("P2fT2f_Texture_Unlit_Fragment.vulkan.json");
	ExpectUboLayout();
	ExpectPushConstant();

	EXPECT_EQ(_set->GetSpace(), 0u);

	ASSERT_EQ(_set->GetUboBlocks().Size(), 1u);
	EXPECT_EQ(FindUbo(*_set, "ubo")->_binding, 0u);

	ExpectTextureBlocks(1, 2);
}

// Buffers, textures and samplers are numbered separately, in a register space of their own:
// the first one is taken by "pc", a constant buffer known as the push constant by its [PushConstant] attribute
TEST_F(ShaderReflection, D3d12)
{
	Load("P2fT2f_Texture_Unlit_Fragment.d3d12.json");
	ExpectUboLayout();
	ExpectPushConstant();

	EXPECT_EQ(_set->GetSpace(), 1u);

	ASSERT_EQ(_set->GetUboBlocks().Size(), 1u);
	EXPECT_EQ(FindUbo(*_set, "pc"), nullptr);
	EXPECT_EQ(FindUbo(*_set, "ubo")->_binding, 0u);

	// Same register index, told apart by their type
	ExpectTextureBlocks(0, 0);
}

// The block is an argument buffer, at the buffer index after the one of "pc": the binding of a field is its offset in it
TEST_F(ShaderReflection, Metal)
{
	Load("P2fT2f_Texture_Unlit_Fragment.metal.json");
	ExpectUboLayout();
	ExpectPushConstant();

	EXPECT_EQ(_set->GetSpace(), 1u);

	ASSERT_EQ(_set->GetUboBlocks().Size(), 1u);
	EXPECT_EQ(FindUbo(*_set, "pc"), nullptr);
	EXPECT_EQ(FindUbo(*_set, "ubo")->_binding, 0u);

	ExpectTextureBlocks(8, 16);
}

// What a pipeline does with the sets of its shaders
TEST_F(ShaderReflection, MergeKeepsTextureAndSamplerSharingAnIndex)
{
	Load("P2fT2f_Texture_Unlit_Fragment.d3d12.json");

	ShaderSetDescriptor merged;
	merged.Merge(*_set);
	merged.Merge(*_set); // the other stage declaring the same blocks

	EXPECT_EQ(merged.GetSpace(), 1u);
	EXPECT_EQ(merged.GetUboBlocks().Size(), 1u);
	ASSERT_EQ(merged.GetTextureBlocks().Size(), 2u);
	EXPECT_NE(FindTexture(merged, "image"), nullptr);
	EXPECT_NE(FindTexture(merged, "imageSampler"), nullptr);
}

// The parameters declared outside of any ParameterBlock, as the shaders did before using them, still make a set.
// The fixture is the reflection of the same shader for Vulkan, when it declared "ubo", "image" and "imageSampler" that way.
TEST_F(ShaderReflection, ParametersOutsideOfAnyBlockMakeOneSet)
{
	Load("LooseParameters_Fragment.vulkan.json");
	ExpectUboLayout();

	EXPECT_EQ(_set->GetSpace(), 0u);

	ASSERT_EQ(_set->GetUboBlocks().Size(), 1u);
	EXPECT_EQ(FindUbo(*_set, "ubo")->_binding, 0u);

	ExpectTextureBlocks(1, 2);
}

// Plain data in a block goes to a constant buffer only some targets add to it: it has to be declared, as a ConstantBuffer field.
// The fixture is the reflection, for Vulkan, of: struct Material { float4 color; Texture2D image; }; ParameterBlock<Material> material;
TEST(ShaderReflectionErrors, PlainDataInParameterBlockIsRefused)
{
	std::string reflection = ReadFixture("PlainDataInParameterBlock_Fragment.vulkan.json");
	ASSERT_FALSE(reflection.empty());

	hod::OutputBucket outputs;
	hod::OutputService::PushBucket(outputs);

	ReflectedShader shader;
	EXPECT_FALSE(shader.LoadFromIR(nullptr, 0, reflection.c_str(), (uint32_t)reflection.size()));

	hod::OutputService::PopBucket();

	uint32_t errorCount = 0;
	for (const hod::Output& output : outputs.GetOutputs())
	{
		if (output.GetType() == hod::Output::Type::Error)
		{
			++errorCount;
		}
	}
	EXPECT_EQ(errorCount, 1u);
}
