#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Shader.hpp"

namespace MTL
{
    class Library;
    class Function;
}

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API MetalShader : public Shader
		{
		public:

                            MetalShader(ShaderType type);
                            ~MetalShader() override;
            
        public:
            
            MTL::Library*   GetNativeLibrary() const;
            MTL::Function*  GetNativeFunction() const;

		protected:

			bool	        LoadInternal(const void* data, uint32_t size) override;
            
        private:
            
            MTL::Library*   _library = nullptr;
            MTL::Function*  _function = nullptr;
		};
	}
}
