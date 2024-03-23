#pragma once

#include <string>
#include <vector>

namespace hod
{
	class Stream;

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

			bool							LoadFromSource(std::istream& stream);

			bool							LoadFromFile(const std::string& path);
			bool							LoadFromMemory(const void* data, uint32_t size);
			bool							LoadFromStream(std::istream& stream);

			const std::vector<uint8_t>&		GetShaderBytecode() const;

		protected:

			virtual bool					LoadInternal(const void* data, uint32_t size) = 0;

		protected:

			std::vector<uint8_t>			_buffer;

		private:

			ShaderType						_type;
		};
	}
}
