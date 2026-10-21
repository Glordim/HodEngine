#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"
#include "HodEngine/RHI/Shader.hpp"

#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	DESCRIBE_REFLECTED_ENUM(GraphicsPipeline::PolygonMode, reflectionDescriptor)
	{
		// constexpr auto names = EnumTrait::GetEnumNames<Entity::InternalState, 0, 1>();

		reflectionDescriptor.AddEnumValue(GraphicsPipeline::PolygonMode::Fill, "Fill");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::PolygonMode::Line, "Line");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::PolygonMode::Point, "Point");
	}

	DESCRIBE_REFLECTED_ENUM(GraphicsPipeline::Topology, reflectionDescriptor)
	{
		// constexpr auto names = EnumTrait::GetEnumNames<Entity::InternalState, 0, 1>();

		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topology::POINT, "Point");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topology::LINE, "Line");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topology::LINE_STRIP, "LineStrip");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topology::TRIANGLE, "Triangle");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topology::TRIANGLE_FAN, "TriangleFan");
	}

	GraphicsPipeline::GraphicsPipeline() {}

	GraphicsPipeline::~GraphicsPipeline()
	{
		for (const auto& pair : _setDescriptors)
		{
			DefaultAllocator::GetInstance().Delete(pair.second);
		}
	}

	/// @brief Builds the sets of the pipeline from the ones each of its shaders declares.
	/// Sets no shader declares are added empty, so that the sets of a pipeline always go from 0 to GetSetDescriptors().size() - 1.
	/// @param vertexShader
	/// @param fragmentShader
	void GraphicsPipeline::MergeSetDescriptors(const Shader& vertexShader, const Shader& fragmentShader)
	{
		for (const Shader* shader : {&vertexShader, &fragmentShader})
		{
			for (const auto& pair : shader->GetSetDescriptors())
			{
				auto it = _setDescriptors.find(pair.first);
				if (it == _setDescriptors.end())
				{
					it = _setDescriptors.emplace(pair.first, DefaultAllocator::GetInstance().New<ShaderSetDescriptor>()).first;
				}
				it->second->Merge(*pair.second);
				it->second->SetStageSpace(shader->GetShaderType(), pair.second->GetSpace());
			}
		}

		if (_setDescriptors.empty() == false)
		{
			uint32_t lastSet = _setDescriptors.rbegin()->first;
			for (uint32_t set = 0; set < lastSet; ++set)
			{
				if (_setDescriptors.find(set) == _setDescriptors.end())
				{
					ShaderSetDescriptor* setDescriptor = DefaultAllocator::GetInstance().New<ShaderSetDescriptor>();
					setDescriptor->SetSpace(set);
					_setDescriptors[set] = setDescriptor;
				}
			}
		}
	}

	/// @brief
	/// @return
	const std::map<uint32_t, ShaderSetDescriptor*>& GraphicsPipeline::GetSetDescriptors() const
	{
		return _setDescriptors;
	}

	/// @brief
	/// @param createInfo
	/// @return
	bool GraphicsPipeline::SetTargetFormats(const CreateInfo& createInfo)
	{
		if (createInfo._colorFormat != PixelFormat::None && createInfo._colorFormat != PixelFormat::R8G8B8A8_UNorm)
		{
			OUTPUT_ERROR("GraphicsPipeline: the color format is not one of a color attachment");
			return false;
		}
		if (createInfo._depthFormat != PixelFormat::None && createInfo._depthFormat != PixelFormat::D16_UNorm)
		{
			OUTPUT_ERROR("GraphicsPipeline: the depth format is not one of a depth attachment");
			return false;
		}
		if ((createInfo._depthTest || createInfo._depthWrite) && createInfo._depthFormat == PixelFormat::None)
		{
			OUTPUT_ERROR("GraphicsPipeline: depth test or write, without a depth format to draw into");
			return false;
		}

		_colorFormat = createInfo._colorFormat;
		_depthFormat = createInfo._depthFormat;
		return true;
	}

	/// @brief
	/// @return
	PixelFormat GraphicsPipeline::GetColorFormat() const
	{
		return _colorFormat;
	}

	/// @brief
	/// @return
	PixelFormat GraphicsPipeline::GetDepthFormat() const
	{
		return _depthFormat;
	}
}
