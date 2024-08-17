#pragma once
#include "HodEngine/Game/Export.hpp"
#include <memory>

#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod
{
    class Resource;
}

namespace hod::game
{
    /// @brief 
    class HOD_GAME_API WeakResourceBase
    {
        REFLECTED_CLASS_NO_VIRTUAL(WeakResourceBase, HOD_GAME_API)

    public:

                            WeakResourceBase(ReflectionDescriptor* resourceDescriptor);
                            WeakResourceBase(ReflectionDescriptor* resourceDescriptor, const std::shared_ptr<Resource>& pointer);
        virtual             ~WeakResourceBase();

        WeakResourceBase&   operator = (const WeakResourceBase& copy);
        WeakResourceBase&   operator = (const std::weak_ptr<Resource>& pointer);

        bool                operator==(const WeakResourceBase& other) const;

    public:

        std::shared_ptr<Resource>   Lock() const;
        const UID&                  GetUid() const;
        const UID&                  GetForSerialization() const;

        ReflectionDescriptor*       GetResourceDescriptor() const;

        void                        SetUid(const UID& uid);

    private:

        ReflectionDescriptor*           _resourceDescriptor = nullptr;
        
        UID                                 _uid;
        mutable std::shared_ptr<Resource>   _pointer;
    };

    /// @brief 
    /// @tparam _Resource_ 
    template<typename _Resource_>
    class WeakResource : public WeakResourceBase
    {
    public:

        WeakResource()
        : WeakResourceBase(_Resource_::GetReflectionDescriptor())
        {
        }

        /// @brief 
        /// @param pointer 
        WeakResource(const std::shared_ptr<_Resource_>& pointer)
        : WeakResourceBase(_Resource_::GetReflectionDescriptor(), pointer)
        {
        }

        /// @brief 
        ~WeakResource()
        {
        }

        /// @brief 
        /// @return 
        std::shared_ptr<_Resource_> Lock() const
        {
            return std::static_pointer_cast<_Resource_>(WeakResourceBase::Lock());
        }
    };
}
