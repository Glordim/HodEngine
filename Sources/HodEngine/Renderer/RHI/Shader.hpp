#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace hod
{
	class Stream;

	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API Shader
		{
		public:

			enum ShaderType
			{
				Vertex,
				Geometry,
				Fragment,
				Compute
			};

		public:
											Shader(ShaderType type);
			virtual							~Shader();

			const std::vector<uint8_t>&		GetShaderBytecode() const;
            
            ShaderType                      GetShaderType() const;

			virtual bool					LoadFromIR(const void* data, uint32_t size) = 0;
			virtual bool					LoadFromSource(std::string_view source) = 0;

		protected:

			std::vector<uint8_t>			_buffer;

		private:

			ShaderType						_type;
		};
	}
}
