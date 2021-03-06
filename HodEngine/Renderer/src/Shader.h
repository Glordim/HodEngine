#pragma once

#include <string>

namespace HOD
{
	namespace RENDERER
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
			virtual			~Shader();

			virtual bool	LoadFromFile(const std::string& path) = 0;

		private:
			ShaderType		_type;
		};
	}
}
