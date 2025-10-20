#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <map>

#include "HodEngine/Core/Math/Matrix4.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Math/Vector4.hpp"

namespace hod
{
	namespace renderer
	{
		class Texture;
		class Material;

		//-----------------------------------------------------------------------------
		//! @brief
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API MaterialInstance
		{
		public:
			MaterialInstance(const Material& material);
			MaterialInstance(const MaterialInstance&) = delete;
			MaterialInstance(MaterialInstance&&) = delete;
			virtual ~MaterialInstance();

			void operator=(const MaterialInstance&) = delete;
			void operator=(MaterialInstance&&) = delete;

		public:
			const Material& GetMaterial() const;

			void SetInt(const String& memberName, int value);
			void SetFloat(const String& memberName, float value);
			void SetVec2(const String& memberName, const Vector2& value);
			void SetVec4(const String& memberName, const Vector4& value);
			void SetMat4(const String& memberName, const Matrix4& value);
			void SetTexture(const String& memberName, const Texture* value);

			int            GetInt(const String& memberName);
			float          GetFloat(const String& memberName);
			const Vector2& GetVec2(const String& memberName);
			const Vector4& GetVec4(const String& memberName);
			const Matrix4& GetMat4(const String& memberName);
			const Texture* GetTexture(const String& memberName);

		protected:
			virtual void ApplyInt(const String& memberName, int value) = 0;
			virtual void ApplyFloat(const String& memberName, float value) = 0;
			virtual void ApplyVec2(const String& memberName, const Vector2& value) = 0;
			virtual void ApplyVec4(const String& memberName, const Vector4& value) = 0;
			virtual void ApplyMat4(const String& memberName, const Matrix4& value) = 0;
			virtual void ApplyTexture(const String& memberName, const Texture& value) = 0;

			const std::map<String, int>&            GetIntMap() const;
			const std::map<String, float>&          GetFloatMap() const;
			const std::map<String, Vector2>&        GetVec2Map() const;
			const std::map<String, Vector4>&        GetVec4Map() const;
			const std::map<String, Matrix4>&        GetMat4Map() const;
			const std::map<String, const Texture*>& GetTextureMap() const;

		private:
			std::map<String, int>            _intMap;
			std::map<String, float>          _floatMap;
			std::map<String, Vector2>        _vec2Map;
			std::map<String, Vector4>        _vec4Map;
			std::map<String, Matrix4>        _mat4Map;
			std::map<String, const Texture*> _textureMap;

			const Material& _material;
		};
	}
}
