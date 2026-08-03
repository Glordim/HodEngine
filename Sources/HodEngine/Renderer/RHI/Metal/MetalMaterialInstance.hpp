#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

namespace MTL4
{
    class RenderCommandEncoder;
    class ArgumentTable;
}

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_API MetalMaterialInstance : public MaterialInstance
	{
	public:

										MetalMaterialInstance(const Material& material);
										~MetalMaterialInstance() override;
		
		void                            FillCommandEncoder(MTL4::RenderCommandEncoder* renderCommandEncoder, MTL4::ArgumentTable* fragmentArgumentTable) const;

	protected:

		void							ApplyInt(const String& memberName, int value) override;
		void							ApplyFloat(const String& memberName, float value) override;
		void							ApplyVec2(const String& memberName, const Vector2& value) override;
		void							ApplyVec4(const String& memberName, const Vector4& value) override;
		void							ApplyMat4(const String& memberName, const Matrix4& value) override;
		void							ApplyTexture(const String& name, const Texture& value) override;
	};
}
