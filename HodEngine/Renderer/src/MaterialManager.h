#pragma once

#include <string>
#include <unordered_map>

namespace HOD
{
	namespace RENDERER
	{
		class Material;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MaterialManager
		{
		public:
			static MaterialManager*						GetInstance();
			static void									DestroyInstance();

			Material*									GetMaterial(const std::string& shaderName, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true);
			void										DestroyMaterial(const std::string& shaderName);

		private:
														MaterialManager();
														MaterialManager(const MaterialManager& copy) = delete;
			void										operator=(const MaterialManager& right) = delete;
			virtual										~MaterialManager();

			static MaterialManager*						_instance;

			std::unordered_map<std::string, Material*>	_shaderNameToMaterialMap;
		};
	}
}
