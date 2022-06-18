#pragma once

#include <map>
#include <string>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

namespace hod
{
	namespace renderer
	{
		class Texture;
		class Material;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MaterialInstance
		{
		public:
													MaterialInstance(const Material& material);
													MaterialInstance(const MaterialInstance&) = delete;
													MaterialInstance(MaterialInstance&&) = delete;
			virtual									~MaterialInstance();

			void									operator=(const MaterialInstance&) = delete;
			void									operator=(MaterialInstance&&) = delete;

		public:

			const Material&							GetMaterial() const;

			void									SetInt(const std::string& memberName, int value);
			void									SetFloat(const std::string& memberName, float value);
			void									SetVec4(const std::string& memberName, const glm::vec4& value);
			void									SetMat4(const std::string& memberName, const glm::mat4& value);
			void									SetTexture(const std::string& memberName, const Texture* value);

			int										GetInt(const std::string& memberName);
			float									GetFloat(const std::string& memberName);
			const glm::vec4&						GetVec4(const std::string& memberName);
			const glm::mat4&						GetMat4(const std::string& memberName);
			const Texture*							GetTexture(const std::string& memberName);

		protected:

			virtual void							ApplyInt(const std::string& memberName, int value) = 0;
			virtual void							ApplyFloat(const std::string& memberName, float value) = 0;
			virtual void							ApplyVec4(const std::string& memberName, const glm::vec4& value) = 0;
			virtual void							ApplyMat4(const std::string& memberName, const glm::mat4& value) = 0;
			virtual void							ApplyTexture(const std::string& memberName, const Texture& value) = 0;

		private:

			std::map<std::string, int>				_intMap;
			std::map<std::string, float>			_floatMap;
			std::map<std::string, glm::vec4>		_vec4Map;
			std::map<std::string, glm::mat4x4>		_mat4Map;
			std::map<std::string, const Texture*>	_textureMap;

			const Material&							_material;
		};
	}
}
