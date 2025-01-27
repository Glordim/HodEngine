#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Renderer/Enums.hpp"
#include "HodEngine/Renderer/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Math/Vector4.hpp"
#include "HodEngine/Core/Resource/WeakResource.hpp"

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

			for (ReflectionDescriptor* reflectionDescriptor : _paramsSubReflectionDescriptors)
			{
				delete reflectionDescriptor;
			}
			/*
			for (const auto& pair : _setDescriptors)
			{
				delete pair.second;
			}
			*/
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

		/// @brief 
		/// @return 
		const std::unordered_map<uint32_t, ShaderSetDescriptor*>& Material::GetSetDescriptors() const
		{
			return _setDescriptors;
		}

		/// @brief 
		/// @return 
		ReflectionDescriptor& Material::GetReflectionDescriptorForParameters()
		{
			if (_paramsReflectionDescriptorGenerated == false)
			{
				uint32_t offset = 0;
				for (const auto& pair : _setDescriptors)
				{
					for (const ShaderSetDescriptor::BlockUbo& ubo : pair.second->GetUboBlocks())
					{
						uint32_t uboOffset = 0;
						ReflectionDescriptor* uboReflectionDescriptor = new ReflectionDescriptor();
						for (const auto& childPair : ubo._rootMember._childsMap)
						{
							if (childPair.second._memberType == ShaderSetDescriptor::BlockUbo::MemberType::Float)
							{
								uboReflectionDescriptor->AddProperty<ReflectionPropertyVariable>(ReflectionPropertyVariable::Type::Float32, uboOffset, childPair.second._name.c_str(), nullptr, nullptr);
								uboOffset += 1 * sizeof(float);
							}
							else if (childPair.second._memberType == ShaderSetDescriptor::BlockUbo::MemberType::Float2)
							{
								uboReflectionDescriptor->AddProperty<ReflectionPropertyObject>(uboOffset, childPair.second._name.c_str(), &Vector2::GetReflectionDescriptor(), nullptr, nullptr);
								uboOffset += 2 * sizeof(float);
							}
							else if (childPair.second._memberType == ShaderSetDescriptor::BlockUbo::MemberType::Float4)
							{
								uboReflectionDescriptor->AddProperty<ReflectionPropertyObject>(uboOffset, childPair.second._name.c_str(), &Vector4::GetReflectionDescriptor(), nullptr, nullptr);
								uboOffset += 4 * sizeof(float);
							}
						}
						_paramsSubReflectionDescriptors.push_back(uboReflectionDescriptor);
						_paramsReflectionDescriptor.AddProperty<ReflectionPropertyObject>(offset, ubo._name.c_str(), uboReflectionDescriptor, nullptr, nullptr);
						offset += uboOffset;
					}

					for (const ShaderSetDescriptor::BlockTexture& texture : pair.second->GetTextureBlocks())
					{
						if (texture._type == ShaderSetDescriptor::BlockTexture::Texture)
						{
							_paramsReflectionDescriptor.AddProperty<ReflectionPropertyObject>(offset, texture._name.c_str(), &WeakResource<TextureResource>::GetReflectionDescriptor(), nullptr, nullptr);
							offset += sizeof(WeakResource<TextureResource>);
						}
					}
				}

				_paramsReflectionDescriptorGenerated = true;
			}

			return _paramsReflectionDescriptor;
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
