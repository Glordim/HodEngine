#include "Error.h"

namespace HOD
{
    Error::Error()
    {
    }

    Error::~Error()
    {
    }

    void Error::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
    {
    }
}
