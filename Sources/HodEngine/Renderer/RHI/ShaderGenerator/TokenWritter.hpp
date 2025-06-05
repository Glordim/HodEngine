#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Core/String.hpp"

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
	
		bool	PrettyWrite(std::ostream& stream, const Vector<Token>& tokens);
        bool	CompactWrite(std::ostream& stream, const Vector<Token>& tokens);
    };
}
