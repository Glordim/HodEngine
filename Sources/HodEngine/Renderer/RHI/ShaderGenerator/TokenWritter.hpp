#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <vector>
#include <string>

#include "Token.hpp"

#include <ostream>

namespace hod::renderer
{
    class HOD_RENDERER_API TokenWritter
    {
    public:
		TokenWritter() = default;
		TokenWritter(const TokenWritter&) = delete;
		TokenWritter(TokenWritter&&) = delete;
		~TokenWritter() = default;

		TokenWritter&    operator=(const TokenWritter&) = delete;
		TokenWritter&    operator=(TokenWritter&&) = delete;

	public:
	
		bool	PrettyWrite(std::ostream& stream, const std::vector<Token>& tokens);
        bool	CompactWrite(std::ostream& stream, const std::vector<Token>& tokens);
    };
}
