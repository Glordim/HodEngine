#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"

#include <cstdint>
#include <map>
#include <unordered_map>

namespace hod::inline rhi
{
	class Shader;
	class ShaderSetDescriptor;
	class VertexInput;
}

namespace hod::inline renderer
{
	/// @brief A GraphicsPipeline and what is needed to address its parameters by name.
	/// Holds no parameter value: those belong to the MaterialInstance.
	class HOD_RENDERER_API Material
	{
	public:
		using PolygonMode = GraphicsPipeline::PolygonMode;
		using Topololy = GraphicsPipeline::Topololy;

		/// @brief Where a uniform value lives: bytes [_offset, _offset + _size) of the uniform block '_block' of the set '_set'
		struct UniformLocation
		{
			uint32_t _set = 0;
			uint32_t _block = 0; // index in ShaderSetDescriptor::GetUboBlocks
			uint32_t _offset = 0;
			uint32_t _size = 0;
		};

		/// @brief
		struct TextureLocation
		{
			uint32_t _set = 0;
			uint32_t _block = 0; // index in ShaderSetDescriptor::GetTextureBlocks
		};

	public:
		// The shaders are not owned, they must outlive the Material
		static Material* Create(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
		                        PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true);

		Material() = default;
		Material(const Material&) = delete;
		Material(Material&&) = delete;
		~Material();

		Material& operator=(const Material&) = delete;
		Material& operator=(Material&&) = delete;

	public:
		GraphicsPipeline* GetGraphicsPipeline() const;

		// Ordered and contiguous: the sets 0, 1, 2...
		const std::map<uint32_t, ShaderSetDescriptor*>& GetSetDescriptors() const;

		// path: "block", "block.member", "block.member.subMember", "block.member[2]"...
		bool FindUniform(const String& path, UniformLocation& location) const;
		bool FindTexture(const String& name, TextureLocation& location) const;

		// Once per Material: reports a texture drawn without ever being set
		void ReportUnsetTexture(const String& name) const;

	private:
		bool Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode, Topololy topololy,
		           bool useDepth);

		bool ResolveUniform(const String& path, UniformLocation& location) const;

	private:
		GraphicsPipeline* _graphicsPipeline = nullptr;

		// Every path without array index, resolved once: read only after Build, hence safe to read from several threads
		std::unordered_map<String, UniformLocation> _uniformLocations;
		std::unordered_map<String, TextureLocation> _textureLocations;
	};
}
