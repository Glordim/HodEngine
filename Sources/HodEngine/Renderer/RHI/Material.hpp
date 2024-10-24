#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <map>
#include <string>
#include <cstdint>

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
		class HOD_RENDERER_API Material
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
				LINE_STRIP,
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
