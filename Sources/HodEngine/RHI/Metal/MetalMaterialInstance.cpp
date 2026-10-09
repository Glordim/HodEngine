#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/GraphicsPipelineMetal.hpp"
#include "HodEngine/RHI/Metal/MetalMaterialInstance.hpp"
#include "HodEngine/RHI/Metal/MetalTexture.hpp"

#include <Metal/Metal.hpp>

namespace hod::inline rhi
{
	/// @brief
	/// @param graphicsPipeline
	MetalMaterialInstance::MetalMaterialInstance(const GraphicsPipeline& graphicsPipeline)
	: LegacyMaterialInstance(graphicsPipeline)
	{
		/*
		static_cast<GraphicsPipelineMetal*>(graphicsPipeline);
		MTL::RenderPipelineState* pipelineState =
			*/
	}

	/// @brief
	MetalMaterialInstance::~MetalMaterialInstance() {}

	/// @brief
	/// @param memberName
	/// @param value
	void MetalMaterialInstance::ApplyInt(const String& memberName, int value)
	{
		// TODO
		(void)memberName;
		(void)value;
	}

	/// @brief
	/// @param memberName
	/// @param value
	void MetalMaterialInstance::ApplyFloat(const String& memberName, float value)
	{
		// TODO
		(void)memberName;
		(void)value;
	}

	/// @brief
	/// @param memberName
	/// @param value
	void MetalMaterialInstance::ApplyVec2(const String& memberName, const Vector2& value)
	{
		// TODO
		(void)memberName;
		(void)value;
	}

	/// @brief
	/// @param memberName
	/// @param value
	void MetalMaterialInstance::ApplyVec4(const String& memberName, const Vector4& value)
	{
		// TODO
		(void)memberName;
		(void)value;
	}

	/// @brief
	/// @param memberName
	/// @param value
	void MetalMaterialInstance::ApplyMat4(const String& memberName, const Matrix4& value)
	{
		// TODO
		(void)memberName;
		(void)value;
	}

	/// @brief
	/// @param name
	/// @param value
	void MetalMaterialInstance::ApplyTexture(const String& name, const Texture& value)
	{
		// TODO
		(void)name;
		(void)value;
	}

	/// @brief
	/// @param renderCommandEncoder
	/// @param fragmentArgumentTable
	void MetalMaterialInstance::FillCommandEncoder(MTL4::RenderCommandEncoder* renderCommandEncoder, MTL4::ArgumentTable* fragmentArgumentTable) const
	{
		const GraphicsPipelineMetal& graphicsPipeline = static_cast<const GraphicsPipelineMetal&>(GetGraphicsPipeline());
		renderCommandEncoder->setRenderPipelineState(graphicsPipeline.GetNativeRenderPipeline());

		const std::map<String, const Texture*>& textureMap = GetTextureMap();
		for (const auto& texturePair : textureMap)
		{
			uint32_t index = 0; // graphicsPipeline.GetTextureIndex(texturePair.first);

			const MetalTexture* texture = static_cast<const MetalTexture*>(texturePair.second);
			fragmentArgumentTable->setTexture(texture->GetNativeTexture()->gpuResourceID(), index);
			fragmentArgumentTable->setSamplerState(texture->GetNativeSampler()->gpuResourceID(), index);
		}
	}
}
