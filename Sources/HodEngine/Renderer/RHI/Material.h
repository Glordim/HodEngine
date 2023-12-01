#pragma once

#include <map>
#include <string>

namespace hod
{
	namespace renderer
	{
		class Shader;
		class Texture;
		class VertexInput;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Material
		{
		public:

			enum PolygonMode
			{
				Fill,
				Line,
				Point,
			};

			enum Topololy
			{
				POINT,
				LINE,
				TRIANGLE,
				TRIANGLE_FAN,
			};

			Material();
			virtual ~Material();

			virtual bool Build(VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) = 0;

			bool link(Shader* vertexShader, Shader* fragmentShader);
			void use();

		private:

			uint32_t getLocationFromName(const std::string& name);

			std::map<std::string, uint32_t> nameToLocationMap;
			std::map<uint32_t, uint32_t> locationToTextureId;
		};
	}
}
