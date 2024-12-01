#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include <vector>

#include <iostream>
#include <cassert>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Material::Material()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Material::~Material()
		{
			/*

			if (_programId != 0)
			{
				glDeleteProgram(this->programId);
			}

			*/
			delete _defaultInstance;

			for (const auto& pair : _setDescriptors)
			{
				delete pair.second;
			}
		}

		/// @brief 
		void Material::CreateDefaultInstance()
		{
			assert(_defaultInstance == nullptr);
			_defaultInstance = Renderer::GetInstance()->CreateMaterialInstance(this);
		}

		/// @brief 
		/// @return 
		const MaterialInstance*	Material::GetDefaultInstance() const
		{
			return _defaultInstance;
		}

		const std::unordered_map<uint32_t, ShaderSetDescriptor*>& Material::GetSetDescriptors() const
		{
			return _setDescriptors;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		/*
		bool Material::link(Shader* vertexShader, Shader* fragmentShader)
		{

			programId = glCreateProgram();
		   // glAttachShader(this->programId, vertexShader.getShaderId());
			//glAttachShader(this->programId, fragmentShader.getShaderId());
			glLinkProgram(this->programId);

			GLint isLinked = 0;
			glGetProgramiv(this->programId, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(this->programId, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> errorLog(maxLength);
				glGetProgramInfoLog(this->programId, maxLength, &maxLength, &errorLog[0]);

				std::cerr << std::string("Material : Failed to link Shaders") << std::endl;
				std::cerr << std::string(&errorLog[0]) << std::endl;

				glDeleteProgram(this->programId);
				this->programId = 0;

				return false;
			}


			return true;
		}
		*/

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		/*
		void Material::use()
		{

			glUseProgram(this->programId);

			// Rebind texture

			int offset = 0;

			auto it = this->locationToTextureId.begin();
			auto itEnd = this->locationToTextureId.end();

			while (it != itEnd)
			{
				glUniform1i(it->first, offset);

				glActiveTexture(GL_TEXTURE0 + offset);
				glBindTexture(GL_TEXTURE_2D, it->second);

				++offset;
				++it;
			}

		}
		*/

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		/*
		uint32_t Material::getLocationFromName(const std::string& name)
		{

			auto it = this->nameToLocationMap.find(name);
			if (it == this->nameToLocationMap.end())
			{
				GLint location = glGetUniformLocation(this->programId, name.c_str());

				this->nameToLocationMap.emplace(name, location);

				return location;
			}
			else
			{
				return it->second;
			}


			return 0;
		}
		*/
	}
}
