#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"
#include "HodEngine/RHI/Shader.hpp"
#include "HodEngine/RHI/Texture.hpp"

#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

#include "HodEngine/Core/Vector.hpp"

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

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	GraphicsPipeline::GraphicsPipeline() {}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	GraphicsPipeline::~GraphicsPipeline()
	{
		/*

		if (_programId != 0)
		{
			glDeleteProgram(this->programId);
		}

		*/
		for (const auto& pair : _setDescriptors)
		{
			DefaultAllocator::GetInstance().Delete(pair.second);
		}
	}

	/// @brief
	/// @return
	const std::unordered_map<uint32_t, ShaderSetDescriptor*>& GraphicsPipeline::GetSetDescriptors() const
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

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	/*
	bool GraphicsPipeline::link(Shader* vertexShader, Shader* fragmentShader)
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
	void GraphicsPipeline::use()
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
	uint32_t GraphicsPipeline::getLocationFromName(const String& name)
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
