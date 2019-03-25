#ifndef __ERROR_HPP__
#define __ERROR_HPP__

#include <PxPhysicsAPI.h>

class Error : public physx::PxErrorCallback
{
public:
    Error();
    virtual ~Error();

    virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
};

#endif
