#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <iostream>


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
				ApplyTexture(memberName, *Renderer::GetInstance()->GetDefaultWhiteTexture());
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
    
        const std::map<std::string, int>& MaterialInstance::GetIntMap() const
        {
            return _intMap;
        }
    
        const std::map<std::string, float>& MaterialInstance::GetFloatMap() const
        {
            return _floatMap;
        }
    
		const std::map<std::string, Vector2>& MaterialInstance::GetVec2Map() const
        {
            return _vec2Map;
        }

        const std::map<std::string, Vector4>& MaterialInstance::GetVec4Map() const
        {
            return _vec4Map;
        }
    
        const std::map<std::string, Matrix4>& MaterialInstance::GetMat4Map() const
        {
            return _mat4Map;
        }
    
        const std::map<std::string, const Texture*>& MaterialInstance::GetTextureMap() const
        {
            return _textureMap;
        }
	}
}
