#include "Material.h"

#include "Shader.h"

#include "Texture.h"

#include <vector>

#include <iostream>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Material::Material()
			: programId(0)
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
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Material::link(Shader* vertexShader, Shader* fragmentShader)
		{
			/*

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

			*/

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Material::use()
		{
			/*

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

			*/
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		uint32_t Material::getLocationFromName(const std::string& name)
		{
			/*

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

			*/

			return 0;
		}
	}
}
