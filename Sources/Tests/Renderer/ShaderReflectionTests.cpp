#include <gtest/gtest.h>

#include <HodEngine/RHI/Shader.hpp>
#include <HodEngine/RHI/ShaderConstantDescriptor.hpp>
#include <HodEngine/RHI/ShaderSetDescriptor.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace hod;

// ============================================================================
// The reflection of one shader, as slangc emits it for each target.
// The fixtures are the fragment stage of Renderer/Shader/P2fT2f_Texture_Unlit.slang:
//   slangc P2fT2f_Texture_Unlit.slang -stage fragment -entry FragmentMain -reflection-json <fixture> -o <any>
//     -target spirv -profile glsl_450+spirv_1_3    (vulkan)
//     -target metal                                (metal)
//     -target hlsl -profile ps_6_0                 (d3d12)
// It declares, in this order: a push constant "pc", a uniform block "ubo", a texture "image" and its sampler "imageSampler".
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

protected:
	ReflectedShader            _shader;
	const ShaderSetDescriptor* _set = nullptr;
};

// One numbering for everything in the set, and a dedicated push constant
TEST_F(ShaderReflection, Vulkan)
{
	Load("P2fT2f_Texture_Unlit_Fragment.vulkan.json");
	ExpectUboLayout();

	ASSERT_NE(_shader.GetConstantDescriptor(), nullptr);
	EXPECT_EQ(_shader.GetConstantDescriptor()->GetSize(), 64u);
	EXPECT_EQ(_shader.GetConstantDescriptor()->GetShaderType(), Shader::ShaderType::Fragment);

	ASSERT_EQ(_set->GetUboBlocks().Size(), 1u);
	EXPECT_EQ(FindUbo(*_set, "ubo")->_binding, 0u);

	ASSERT_EQ(_set->GetTextureBlocks().Size(), 2u);
	const ShaderSetDescriptor::BlockTexture* image = FindTexture(*_set, "image");
	ASSERT_NE(image, nullptr);
	EXPECT_EQ(image->_type, ShaderSetDescriptor::BlockTexture::Texture);
	EXPECT_EQ(image->_binding, 1u);

	const ShaderSetDescriptor::BlockTexture* imageSampler = FindTexture(*_set, "imageSampler");
	ASSERT_NE(imageSampler, nullptr);
	EXPECT_EQ(imageSampler->_type, ShaderSetDescriptor::BlockTexture::Sampler);
	EXPECT_EQ(imageSampler->_binding, 2u);
}

// Buffers, textures and samplers are numbered separately, and there is no push constant:
// "pc" is an ordinary uniform block that takes the first buffer index
class ShaderReflectionSeparateNumbering : public ShaderReflection, public ::testing::WithParamInterface<const char*>
{
};

TEST_P(ShaderReflectionSeparateNumbering, Blocks)
{
	Load(GetParam());
	ExpectUboLayout();

	EXPECT_EQ(_shader.GetConstantDescriptor(), nullptr);

	ASSERT_EQ(_set->GetUboBlocks().Size(), 2u);
	const ShaderSetDescriptor::BlockUbo* pc = FindUbo(*_set, "pc");
	ASSERT_NE(pc, nullptr);
	EXPECT_EQ(pc->_binding, 0u);
	EXPECT_EQ(pc->_rootMember._size, 64u);
	EXPECT_EQ(FindUbo(*_set, "ubo")->_binding, 1u);

	// Same index, told apart by their type
	ASSERT_EQ(_set->GetTextureBlocks().Size(), 2u);
	const ShaderSetDescriptor::BlockTexture* image = FindTexture(*_set, "image");
	ASSERT_NE(image, nullptr);
	EXPECT_EQ(image->_type, ShaderSetDescriptor::BlockTexture::Texture);
	EXPECT_EQ(image->_binding, 0u);

	const ShaderSetDescriptor::BlockTexture* imageSampler = FindTexture(*_set, "imageSampler");
	ASSERT_NE(imageSampler, nullptr);
	EXPECT_EQ(imageSampler->_type, ShaderSetDescriptor::BlockTexture::Sampler);
	EXPECT_EQ(imageSampler->_binding, 0u);
}

// What a pipeline does with the sets of its shaders
TEST_P(ShaderReflectionSeparateNumbering, MergeKeepsTextureAndSamplerSharingAnIndex)
{
	Load(GetParam());

	ShaderSetDescriptor merged;
	merged.Merge(*_set);
	merged.Merge(*_set); // the other stage declaring the same blocks

	EXPECT_EQ(merged.GetUboBlocks().Size(), 2u);
	ASSERT_EQ(merged.GetTextureBlocks().Size(), 2u);
	EXPECT_NE(FindTexture(merged, "image"), nullptr);
	EXPECT_NE(FindTexture(merged, "imageSampler"), nullptr);
}

INSTANTIATE_TEST_SUITE_P(Targets, ShaderReflectionSeparateNumbering,
                         ::testing::Values("P2fT2f_Texture_Unlit_Fragment.metal.json", "P2fT2f_Texture_Unlit_Fragment.d3d12.json"));
