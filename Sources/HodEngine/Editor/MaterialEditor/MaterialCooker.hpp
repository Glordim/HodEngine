#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

#include <string_view>

namespace hod::inline editor
{
	class HOD_EDITOR_API MaterialCooker : public Cooker
	{
	public:

							MaterialCooker() = default;
							MaterialCooker(const MaterialCooker&) = delete;
							MaterialCooker(MaterialCooker&&) = delete;
							~MaterialCooker() override = default;

		MaterialCooker&	operator = (const MaterialCooker&) = delete;
		MaterialCooker&	operator = (MaterialCooker&&) = delete;

	protected:

		bool	FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;

	private:

		/// @brief Invoke slangc for one shader stage and store its compiled output + reflection json as "<dataBlockName>" / "<dataBlockName>Reflection" data blocks
		bool	CompileSlangStage(const Path& slangRootPath, std::string_view entryPoint, std::string_view stage, std::string_view dataBlockName);
	};
}
