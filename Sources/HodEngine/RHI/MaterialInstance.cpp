#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"
#include "HodEngine/RHI/MaterialInstance.hpp"
#include "HodEngine/RHI/Shader.hpp"
#include "HodEngine/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/RHI/Texture.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	MaterialInstance::MaterialInstance(const GraphicsPipeline& graphicsPipeline)
	: _graphicsPipeline(graphicsPipeline)
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	MaterialInstance::~MaterialInstance() {}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	const GraphicsPipeline& MaterialInstance::GetGraphicsPipeline() const
	{
		return _graphicsPipeline;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void MaterialInstance::SetInt(const String& memberName, int value)
	{
		_intMap[memberName] = value;
		ApplyInt(memberName, value);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void MaterialInstance::SetFloat(const String& memberName, float value)
	{
		_floatMap[memberName] = value;
		ApplyFloat(memberName, value);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void MaterialInstance::SetVec2(const String& memberName, const Vector2& value)
	{
		_vec2Map[memberName] = value;
		ApplyVec2(memberName, value);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void MaterialInstance::SetVec4(const String& memberName, const Vector4& value)
	{
		_vec4Map[memberName] = value;
		ApplyVec4(memberName, value);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void MaterialInstance::SetMat4(const String& memberName, const Matrix4& value)
	{
		_mat4Map[memberName] = value;
		ApplyMat4(memberName, value);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void MaterialInstance::SetTexture(const String& memberName, const Texture* value)
	{
		_textureMap[memberName] = value;

		if (value != nullptr && value->GetWidth() != 0)
		{
			ApplyTexture(memberName, *value);
		}
		else
		{
			ApplyTexture(memberName, *RhiDevice::GetInstance()->GetFallbackTexture());
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	int MaterialInstance::GetInt(const String& memberName)
	{
		return _intMap[memberName];
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	float MaterialInstance::GetFloat(const String& memberName)
	{
		return _floatMap[memberName];
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	const Vector2& MaterialInstance::GetVec2(const String& memberName)
	{
		return _vec2Map[memberName];
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	const Vector4& MaterialInstance::GetVec4(const String& memberName)
	{
		return _vec4Map[memberName];
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	const Matrix4& MaterialInstance::GetMat4(const String& memberName)
	{
		return _mat4Map[memberName];
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	const Texture* MaterialInstance::GetTexture(const String& memberName)
	{
		return _textureMap[memberName];
	}

	/// @brief Called by the backends when binding the sets [setOffset, setOffset + setCount) for a draw.
	/// Those slots still sample the fallback texture, the report only makes the omission visible.
	/// @param setOffset
	/// @param setCount
	void MaterialInstance::ReportUnsetTextures(uint32_t setOffset, uint32_t setCount) const
	{
		if (_graphicsPipeline.HasReportedUnsetTexture())
		{
			return;
		}

		for (const auto& pair : _graphicsPipeline.GetSetDescriptors())
		{
			if (pair.first < setOffset || pair.first - setOffset >= setCount)
			{
				continue;
			}

			for (const ShaderSetDescriptor::BlockTexture& texture : pair.second->GetTextureBlocks())
			{
				if (texture._type != ShaderSetDescriptor::BlockTexture::Sampler && _textureMap.find(texture._name) == _textureMap.end())
				{
					_graphicsPipeline.ReportUnsetTexture(texture._name);
					return;
				}
			}
		}
	}

	const std::map<String, int>& MaterialInstance::GetIntMap() const
	{
		return _intMap;
	}

	const std::map<String, float>& MaterialInstance::GetFloatMap() const
	{
		return _floatMap;
	}

	const std::map<String, Vector2>& MaterialInstance::GetVec2Map() const
	{
		return _vec2Map;
	}

	const std::map<String, Vector4>& MaterialInstance::GetVec4Map() const
	{
		return _vec4Map;
	}

	const std::map<String, Matrix4>& MaterialInstance::GetMat4Map() const
	{
		return _mat4Map;
	}

	const std::map<String, const Texture*>& MaterialInstance::GetTextureMap() const
	{
		return _textureMap;
	}
}
