#pragma once
#include "HodEngine/Audio/Export.hpp"

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Core/Job/MemberFunctionJob.hpp"

namespace hod::audio
{
    class HOD_AUDIO_API AudioManager
    {
        _SingletonAbstract(AudioManager)

    public:

        virtual         ~AudioManager() = default;

        virtual bool    Initialize() = 0;
        virtual void    Terminate() = 0;

        virtual void    Update() = 0;

    protected:

        void            InsertUpdateJob();
        void            RemoveUpdateJob();

    private:

        MemberFunctionJob<AudioManager> _updateJob;
    };
}
