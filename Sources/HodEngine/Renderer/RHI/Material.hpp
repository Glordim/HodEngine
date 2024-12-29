#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <map>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"

namespace hod
{
	namespace renderer
	{
		class Shader;
		class Texture;
		class VertexInput;
		class MaterialInstance;
		class ShaderSetDescriptor;

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

			const std::unordered_map<uint32_t, ShaderSetDescriptor*>&	GetSetDescriptors() const;
			ReflectionDescriptor& GetReflectionDescriptorForParameters();

		protected:

			std::unordered_map<uint32_t, ShaderSetDescriptor*>	_setDescriptors;

		private:

			MaterialInstance*		_defaultInstance = nullptr;

			bool					_paramsReflectionDescriptorGenerated = false;
			ReflectionDescriptor	_paramsReflectionDescriptor;
			std::vector<ReflectionDescriptor*> _paramsSubReflectionDescriptors;

		//uint32_t				getLocationFromName(const std::string& name);
		};
	}
}
