#include "MaterialInstance.h"

#include "Shader.h"

#include "Texture.h"

#include <vector>

#include <iostream>

#include "HodEngine/Renderer/src/Renderer.h"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance::MaterialInstance(const Material& material)
			: _material(material)
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance::~MaterialInstance()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const Material& MaterialInstance::GetMaterial() const
		{
			return _material;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MaterialInstance::SetInt(const std::string& memberName, int value)
		{
			_intMap[memberName] = value;
			ApplyInt(memberName, value);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MaterialInstance::SetFloat(const std::string& memberName, float value)
		{
			_floatMap[memberName] = value;
			ApplyFloat(memberName, value);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MaterialInstance::SetVec4(const std::string& memberName, const glm::vec4& value)
		{
			_vec4Map[memberName] = value;
			ApplyVec4(memberName, value);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MaterialInstance::SetMat4(const std::string& memberName, const glm::mat4& value)
		{
			_mat4Map[memberName] = value;
			ApplyMat4(memberName, value);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MaterialInstance::SetTexture(const std::string& memberName, const Texture* value)
		{
			_textureMap[memberName] = value;

			if (value != nullptr)
			{
				ApplyTexture(memberName, *value);
			}
			else
			{
				ApplyTexture(memberName, *Renderer::GetInstance()->GetDefaultWhiteTexture());
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		int MaterialInstance::GetInt(const std::string& memberName)
		{
			return _intMap[memberName];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		float MaterialInstance::GetFloat(const std::string& memberName)
		{
			return _floatMap[memberName];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::vec4& MaterialInstance::GetVec4(const std::string& memberName)
		{
			return _vec4Map[memberName];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::mat4& MaterialInstance::GetMat4(const std::string& memberName)
		{
			return _mat4Map[memberName];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const Texture* MaterialInstance::GetTexture(const std::string& memberName)
		{
			return _textureMap[memberName];
		}
	}
}
