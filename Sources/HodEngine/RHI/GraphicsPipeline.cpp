#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"
#include "HodEngine/RHI/Shader.hpp"

#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

namespace hod::inline rhi
{
	DESCRIBE_REFLECTED_ENUM(GraphicsPipeline::PolygonMode, reflectionDescriptor)
	{
		// constexpr auto names = EnumTrait::GetEnumNames<Entity::InternalState, 0, 1>();

		reflectionDescriptor.AddEnumValue(GraphicsPipeline::PolygonMode::Fill, "Fill");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::PolygonMode::Line, "Line");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::PolygonMode::Point, "Point");
	}

	DESCRIBE_REFLECTED_ENUM(GraphicsPipeline::Topololy, reflectionDescriptor)
	{
		// constexpr auto names = EnumTrait::GetEnumNames<Entity::InternalState, 0, 1>();

		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topololy::POINT, "Point");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topololy::LINE, "Line");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topololy::LINE_STRIP, "LineStrip");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topololy::TRIANGLE, "Triangle");
		reflectionDescriptor.AddEnumValue(GraphicsPipeline::Topololy::TRIANGLE_FAN, "TriangleFan");
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
					ShaderSetDescriptor* setDescriptor = DefaultAllocator::GetInstance().New<ShaderSetDescriptor>();
					setDescriptor->Merge(*pair.second);
					_setDescriptors[pair.first] = setDescriptor;
				}
				else
				{
					it->second->Merge(*pair.second);
				}
			}
		}

		if (_setDescriptors.empty() == false)
		{
			uint32_t lastSet = _setDescriptors.rbegin()->first;
			for (uint32_t set = 0; set < lastSet; ++set)
			{
				if (_setDescriptors.find(set) == _setDescriptors.end())
				{
					_setDescriptors[set] = DefaultAllocator::GetInstance().New<ShaderSetDescriptor>();
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
	/// @return
	bool GraphicsPipeline::HasReportedUnsetTexture() const
	{
		return _unsetTextureReported;
	}

	/// @brief
	/// @param name
	void GraphicsPipeline::ReportUnsetTexture(const String& name) const
	{
		if (_unsetTextureReported == false)
		{
			_unsetTextureReported = true;
			OUTPUT_WARNING("Material: drawn with texture \"{}\" never set, the fallback texture is used instead", name);
		}
	}
}
