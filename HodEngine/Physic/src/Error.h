#pragma once

#include <PxPhysicsAPI.h>

namespace HOD
{
	namespace PHYSIC
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Error : public physx::PxErrorCallback
		{
		public:
					Error();
					~Error() override;

			void	reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
		};
	}
}
