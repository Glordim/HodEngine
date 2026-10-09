#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/Material.hpp"
#include "HodEngine/RHI/MaterialInstance.hpp"
#include "HodEngine/RHI/Shader.hpp"
#include "HodEngine/RHI/Texture.hpp"

#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <cassert>

namespace hod::inline rhi
{
	DESCRIBE_REFLECTED_ENUM(Material::PolygonMode, reflectionDescriptor)
	{
		// constexpr auto names = EnumTrait::GetEnumNames<Entity::InternalState, 0, 1>();

		reflectionDescriptor.AddEnumValue(Material::PolygonMode::Fill, "Fill");
		reflectionDescriptor.AddEnumValue(Material::PolygonMode::Line, "Line");
		reflectionDescriptor.AddEnumValue(Material::PolygonMode::Point, "Point");
	}

	DESCRIBE_REFLECTED_ENUM(Material::Topololy, reflectionDescriptor)
	{
		// constexpr auto names = EnumTrait::GetEnumNames<Entity::InternalState, 0, 1>();

		reflectionDescriptor.AddEnumValue(Material::Topololy::POINT, "Point");
		reflectionDescriptor.AddEnumValue(Material::Topololy::LINE, "Line");
		reflectionDescriptor.AddEnumValue(Material::Topololy::LINE_STRIP, "LineStrip");
		reflectionDescriptor.AddEnumValue(Material::Topololy::TRIANGLE, "Triangle");
		reflectionDescriptor.AddEnumValue(Material::Topololy::TRIANGLE_FAN, "TriangleFan");
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Material::Material() {}

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
		DefaultAllocator::GetInstance().Delete(_defaultInstance);

		for (const auto& pair : _setDescriptors)
		{
			DefaultAllocator::GetInstance().Delete(pair.second);
		}
	}

	/// @brief
	void Material::CreateDefaultInstance()
	{
		assert(_defaultInstance == nullptr);
		_defaultInstance = RhiDevice::GetInstance()->CreateMaterialInstance(this);
	}

	/// @brief
	/// @return
	const MaterialInstance* Material::GetDefaultInstance() const
	{
		return _defaultInstance;
	}

	/// @brief
	/// @return
	MaterialInstance* Material::EditDefaultInstance()
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
	bool Material::HasReportedUnsetTexture() const
	{
		return _unsetTextureReported;
	}

	/// @brief
	/// @param name
	void Material::ReportUnsetTexture(const String& name) const
	{
		if (_unsetTextureReported == false)
		{
			_unsetTextureReported = true;
			OUTPUT_WARNING("Material: drawn with texture \"{}\" never set, the fallback texture is used instead", name);
		}
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
			Vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(this->programId, maxLength, &maxLength, &errorLog[0]);

			std::cerr << String("Material : Failed to link Shaders") << std::endl;
			std::cerr << String(&errorLog[0]) << std::endl;

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
	uint32_t Material::getLocationFromName(const String& name)
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
