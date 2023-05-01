#pragma once

#include <string>
#include <vector>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Shader
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

			bool							LoadFromFile(const std::string& path);
			virtual bool					LoadFromMemory(void* data, uint32_t size) = 0;

			const std::vector<uint8_t>&		GetShaderBytecode() const;

		protected:

			std::vector<uint8_t>			_buffer;

		private:

			ShaderType						_type;
		};
	}
}
