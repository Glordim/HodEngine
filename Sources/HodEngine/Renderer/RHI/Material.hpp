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
		class MaterialInstance;

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
			virtual					~Material();

			virtual bool			Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) = 0;

			//bool					link(Shader* vertexShader, Shader* fragmentShader);
			//void					use();

			void					CreateDefaultInstance();
			const MaterialInstance*	GetDefaultInstance() const;

		private:

			MaterialInstance*		_defaultInstance = nullptr;

		//uint32_t				getLocationFromName(const std::string& name);
		};
	}
}
